#
# gdb helper commands and functions for HermitCore debugging
#
#  task & thread tools
#
# Copyright (c) Siemens AG, 2011-2013
# Copyright (c) RWTH-Aaachen, 2016
#
# Authors:
#  Jan Kiszka <jan.kiszka@siemens.com>
#  Daniel Krebs <github@daniel-krebs.net>
#
# Inspired by prior work of Jan Kiszka and adapted by Daniel Krebs
# for HermitCore.
#
# This work is licensed under the terms of the GNU GPL version 2.
#

import gdb


def task_lists():
    task_table = gdb.parse_and_eval("task_table")

    for i in range(task_table.type.range()[1]):
        task = task_table[i]
        if task['status'] != 0:
            yield task

def get_task_by_pid(pid):
    for task in task_lists():
        if int(task['id']) == pid:
            return task
    return None


class HermitTaskByIdFunc(gdb.Function):
    """Find HermitCore task by ID and return the task_t variable.

$hermit_task_by_pid(ID): Given ID, iterate over all tasks of the target and
return that task_t variable which PI matches."""

    def __init__(self):
        super(HermitTaskByIdFunc, self).__init__("hermit_task_by_id")

    def invoke(self, pid):
        task = get_task_by_pid(pid)
        if task:
            return task
        else:
            raise gdb.GdbError("No task with ID " + str(pid))


HermitTaskByIdFunc()


def addressToSymbol(addr):
    try:
        s = gdb.execute("info symbol 0x%x" % int(addr), to_string=True)
    except:
        return ''
    if 'No symbol matches' in s:
        return ''
    else:
        return s.split(' in')[0].replace(' ', '')

cachedSections = {}

def parseSections(section_dict):
    import re
    regex = re.compile('\s*\[\d+\]\s+(?P<start>0x[0-9a-f]+)->(?P<end>0x[0-9a-f]+).+:\s+(?P<section>[\w._-]+)\s+(?P<attributes>[\w\s]+)')
    sections = gdb.execute('maintenance info sections', to_string=True).split('\n')
    for section in sections:
        match = regex.match(section)
        if match:
            name = match.group('section')
            start = match.group('start')
            end = match.group('end')
            attributes = match.group('attributes').split(' ')
            if not name in section_dict:
                section_dict[name] = {}
            section_dict[name]['start'] = start
            section_dict[name]['end'] = end
            section_dict[name]['attributes'] = attributes

def getMemoryAttributes(address):
    if len(cachedSections) == 0:
        parseSections(cachedSections)

    try:
        addr = int(address)
    except:
        return []

    for name,section in cachedSections.items():
        if (addr >= int(section['start'][2:], 16)) and (addr < int(section['end'][2:], 16)):
           return section['attributes']

    return []

def getThreadByCoreId(id):
    for inferiorThread in gdb.selected_inferior().threads():
        # GDB starts indexing at 1
        coreId = inferiorThread.num - 1
        if coreId == id:
            return inferiorThread

def getTaskStatus(task):
    status_desc = {1: 'RDY', 2: 'RUN', 3: 'BLK', 4: 'FIN', 5: 'IDL'}
    return status_desc[int(task['status'])]

def taskIsRunning(task):
    if getTaskStatus(task) == 'RUN':
        return True
    if getTaskStatus(task) == 'IDL':
        currentInferiorThread = gdb.selected_thread()

        try:
            getThreadByCoreId(task['last_core']).switch()
            sym = addressToSymbol(gdb.parse_and_eval('$rsp'))
            if 'boot_stack' in sym:

                return True
        finally:
            currentInferiorThread.switch()

    return False

def coreInIrqHandler(coreId):
    currThread = gdb.selected_thread()

    try:
        getThreadByCoreId(coreId).switch()
        if 'irq_handler' in gdb.execute('bt', to_string=True):
            return True
        else:
            return False
    finally:
        currThread.switch()

class HermitPs(gdb.Command):
    """Dump Hermit tasks."""

    def __init__(self):
        super(HermitPs, self).__init__("hermit-ps", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        # see include/hermit/task_types.h
        status_desc = {1: 'RDY', 2: 'RUN', 3: 'BLK', 4: 'FIN', 5: 'IDL'}

        rowfmt = "{id:>3} | {status:^6} | {last_core:>3} | {prio:>4} | {stack:>10} | {rip:<27}\n"

        header = rowfmt.format(id='ID', status='STATUS', last_core='CPU',
                               prio='PRIO', stack='STACK',
                               rip='INSTRUCTION POINTER')

        gdb.write(header)
        gdb.write((len(header) - 1) * '-' + '\n')

        inferior = gdb.selected_inferior()
        currentInferiorThread = gdb.selected_thread()

        for task in task_lists():

            task_status = status_desc[int(task["status"])]

            rip = ''

            if taskIsRunning(task):

                # switch to inferior thread (cpu) that this task is running on
                getThreadByCoreId(task['last_core']).switch()

                # get instruction pointer from running task and switch back
                rip = str(gdb.parse_and_eval('$pc'))
                currentInferiorThread.switch()

            else:
                rollback = gdb.parse_and_eval('&rollback')

                # find instruction pointer in saved stack
                rip_addr = task['last_stack_pointer'] + 20

                try:
                    rip_val = int(rip_addr.dereference())
                except:
                    rip = 'Stackpointer corrupted'


                if rip_val == rollback:
                    # state was pushed by switch_context(), so real rip
                    # is above pseudo interrupt state
                    rip_addr += 5
                    try:
                        rip_val = int(rip_addr.dereference())
                    except:
                        rip = 'Rollback to invalid address'

                if not rip:
                    rip = "%#x" % rip_val

                    # try to resolve a symbol
                    rip_sym = addressToSymbol(rip_val)
                    if rip_sym:
                        rip += " <%s>" % rip_sym

                    if not 'CODE' in getMemoryAttributes(rip_val):
                        rip += ' (corrupt, not exec)'

                if not rip:
                    rip = 'Stackpointer corrupted'
            
            if coreInIrqHandler(task['last_core']):
                rip = 'IRQ handler running'


            gdb.write(rowfmt.format(
                id=int(task["id"]),
                status=task_status,
                rip=str(rip),
                prio=int(task['prio']),
                last_core=int(task['last_core']),
                stack="{:#x}".format(int(task['stack']))
                ))

        currThread = gdb.selected_thread()
        for core in gdb.selected_inferior().threads():
            core.switch()
            coreId = core.num - 1
            if 'irq_handler' in gdb.execute('bt', to_string=True):
                gdb.write(rowfmt.format(
                    id='',
                    status='IRQ',
                    rip=str(gdb.parse_and_eval('$pc')),
                    prio='',
                    last_core=coreId,
                    stack="-"
                    ))
        currThread.switch()


HermitPs()


class HermitLsSighandler(gdb.Command):
    """List signal handlers per tasks."""

    def __init__(self):
        super(HermitLsSighandler, self).__init__("hermit-ls-sighandler", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):

        rowfmt = "{id:>3} | {signal_handler:<24}\n"

        header = rowfmt.format(id='ID', signal_handler='Signal Handler')

        gdb.write(header)
        gdb.write((len(header) - 1) * '-' + '\n')

        for task in task_lists():

            gdb.write(rowfmt.format(
                id=int(task["id"]),
                signal_handler=str(task['signal_handler']),
                ))

HermitLsSighandler()



def stripSymbol(value):
    s = "%s" % value
    return s.split(' ')[0]

class HermitTaskState:
    def __init__(self, address = None):
        import re
        self.info_reg_regex = re.compile("(?P<register>[\w]+)\s+(?P<value>0x[0-9a-f]+).*")

        if address:
            self.address = address

            self.registers = {
                'gs': self.address + 0,
                'fs': self.address + 1,
                'r15': self.address + 2,
                'r14': self.address + 3,
                'r13': self.address + 4,
                'r12': self.address + 5,
                'r11': self.address + 6,
                'r10': self.address + 7,
                'r9':  self.address + 8,
                'r8':  self.address + 9,
                'rdi': self.address + 10,
                'rsi': self.address + 11,
                'rbp': self.address + 12,
                'rsp': self.address + 13,
                'rbx': self.address + 14,
                'rdx': self.address + 15,
                'rcx': self.address + 16,
                'rax': self.address + 17,
                # int_no
                # error
                'rip':    self.address + 20,
                'cs':     self.address + 21,
                'eflags': self.address + 22,
                # userrsp
                'ss':     self.address + 24,
            }

            # make nice strings out of register values
            for register, valptr in self.registers.items():
                self.registers[register] = stripSymbol(valptr.dereference())

        else:
            self.address = False
            self.info_registers = gdb.execute('info registers', to_string=True)
            self.registers = {}
            for line in self.info_registers.split('\n'):
                match = self.info_reg_regex.match(line)
                if match:
                    self.registers[match.group('register')] = match.group('value')

    def switch(self):
        for register, value in self.registers.items():
            try:
                gdb.execute("set $%s = %s" % (register, value))
            except:
                print("Cannot restore %s=%s, skipping ..." % (register, value))


class HermitTaskBacktrace(gdb.Command):
    """Show backtrace for HermitCore task.

Usage: hermit-bt ID"""

    def __init__(self):
        super(HermitTaskBacktrace, self).__init__("hermit-bt", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        argv = gdb.string_to_argv(arg)
        if len(argv) != 1:
            raise gdb.GdbError("hermit-bt takes one argument")

        task = get_task_by_pid(int(argv[0]))

        if task['status'] == 2:
            gdb.execute('bt')
            return

        current_state = HermitTaskState()

        task_state = HermitTaskState(task['last_stack_pointer'])

        try:
            task_state.switch()
            gdb.execute('bt')
        finally:
            current_state.switch()

HermitTaskBacktrace()

original_state = {}

def saveCurrentState(state):
    curr_thread = gdb.selected_thread()
    for thread in gdb.selected_inferior().threads():
        if not thread.num in state:
            thread.switch()
            state[thread.num] = HermitTaskState()
    curr_thread.switch()

def restoreCurrentState(state):
    curr_thread = gdb.selected_thread()
    for thread in gdb.selected_inferior().threads():
        if thread.num in state:
            thread.switch()
            state[thread.num].switch()
    curr_thread.switch()
    state = {}

class HermitSwitchContext(gdb.Command):
    """Switch current context to given HermitCore task

Usage: hermit-switch-context ID"""

    def __init__(self):
        super(HermitSwitchContext, self).__init__("hermit-switch-context", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        global original_state

        argv = gdb.string_to_argv(arg)
        if len(argv) != 1:
            raise gdb.GdbError("hermit-switch-context takes one argument")

        # save original state to go back to it later
        saveCurrentState(original_state)

        task = get_task_by_pid(int(argv[0]))

        # switch current inferior thread to where task has run last
        for thread in gdb.selected_inferior().threads():
            if (thread.num - 1) == task['last_core']:
                thread.switch()
                break

        # apply it's state
        task_state = HermitTaskState(task['last_stack_pointer'])
        task_state.switch()

HermitSwitchContext()


class HermitRestoreContext(gdb.Command):
    """Restore context to state before it was switched

Usage: hermit-restore-context"""

    def __init__(self):
        super(HermitRestoreContext, self).__init__("hermit-restore-context", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        global original_state

        restoreCurrentState(original_state)

HermitRestoreContext()
