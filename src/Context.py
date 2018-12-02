import logging
import MTM1M3TSController
import Model
import States
import Commands

class Context(object):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, model : Model.Model):
        self.log = logging.getLogger("Context")
        self.sal = sal
        self.model = model
        self.state = States.OfflineState()

    def boot(self, command : Commands.BootCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.boot(command, self.model))

    def update(self, command : Commands.UpdateCommand):
        self.log.debug("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.update(command, self.model))

    def applySetpoint(self, command : Commands.ApplySetpointCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.applySetpoint(command, self.model))

    def shutdown(self, command : Commands.ShutdownCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.shutdown(command, self.model))

    def abort(self, command : Commands.AbortCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.abort(command, self.model))

    def enable(self, command : Commands.EnableCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.enable(command, self.model))

    def disable(self, command : Commands.DisableCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.disable(command, self.model))

    def standby(self, command : Commands.StandbyCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.standby(command, self.model))

    def exitControl(self, command : Commands.ExitControlCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.exitControl(command, self.model))

    def start(self, command : Commands.StartCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.start(command, self.model))

    def enterControl(self, command : Commands.EnterControlCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.enterControl(command, self.model))

    def setValue(self, command : Commands.SetValueCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.setValue(command, self.model))

    def scannerData(self, command : Commands.ScannerDataCommand):
        self.log.info("Executing command %s." % command.name)
        self.processStateCommandResult(self.state.scannerData(command, self.model))

    def processStateCommandResult(self, newState):
        previousState = self.state.name
        if newState == States.States.NoStateTransition:
            return
        if newState == States.States.OfflineState:
            self.state = States.OfflineState()
        elif newState == States.States.StandbyState:
            self.state = States.StandbyState()
        elif newState == States.States.DisabledState:
            self.state = States.DisabledState()
        elif newState == States.States.EnabledState:
            self.state = States.EnabledState()
        elif newState == States.States.FaultState:
            self.state = States.FaultState()
        self.log.info("Transitioning from %s to %s." % (previousState, self.state.name))
        self.sal.logEvent_summaryState(newState)