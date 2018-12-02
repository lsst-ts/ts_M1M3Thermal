import logging
import Context
import queue
import Commands

class Controller(object):
    def __init__(self, context : Context.Context):
        self.log = logging.getLogger("Controller")
        self.context = context
        self.queue = queue.Queue()

    def clear(self):
        self.log.info("Clearing command queue.")
        self.queue = queue.Queue()

    def enqueue(self, command : Commands.Command):
        self.log.debug("Attempting to add %s to the command queue." % command.name)
        if command.validate():
            self.queue.put(command)

    def dequeue(self):
        return self.queue.get()

    def execute(self, command : Commands.Command):
        self.log.debug("Starting to execute %s." % command.name)
        command.ackInProgress()
        if isinstance(command, Commands.UpdateCommand):
            self.context.update(command)
        elif isinstance(command, Commands.BootCommand):
            self.context.boot(command)
        elif isinstance(command, Commands.ApplySetpointCommand):
            self.context.applySetpoint(command)
        elif isinstance(command, Commands.ShutdownCommand):
            self.context.shutdown(command)
        elif isinstance(command, Commands.AbortCommand):
            self.context.abort(command)
        elif isinstance(command, Commands.EnableCommand):
            self.context.enable(command)
        elif isinstance(command, Commands.DisableCommand):
            self.context.disable(command)
        elif isinstance(command, Commands.StandbyCommand):
            self.context.standby(command)
        elif isinstance(command, Commands.ExitControlCommand):
            self.context.exitControl(command)
        elif isinstance(command, Commands.StartCommand):
            self.context.start(command)
        elif isinstance(command, Commands.EnterControlCommand):
            self.context.enterControl(command)
        elif isinstance(command, Commands.SetValueCommand):
            self.context.setValue(command)
        elif isinstance(command, Commands.ScannerDataCommand):
            self.context.scannerData(command)
        else:
            self.log.error("Unhandled command %s. Cannot execute." % command.name)