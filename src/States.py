import logging
import SALPY_MTM1M3TS
import Commands
import Model

class States(object):
    OfflineState = SALPY_MTM1M3TS.SAL__STATE_OFFLINE
    StandbyState = SALPY_MTM1M3TS.SAL__STATE_STANDBY
    DisabledState = SALPY_MTM1M3TS.SAL__STATE_DISABLED
    EnabledState = SALPY_MTM1M3TS.SAL__STATE_ENABLED
    FaultState = SALPY_MTM1M3TS.SAL__STATE_FAULT
    NoStateTransition  = 999

class State(object):
    def __init__(self):
        self.log = logging.getLogger("State")
        self.name = type(self).__name__

    def boot(self, command : Commands.BootCommand, model : Model.Model):
        return self.invalidState(command)

    def update(self, command : Commands.UpdateCommand, model : Model.Model):
        return self.invalidState(command)

    def applySetpoint(self, command : Commands.ApplySetpointCommand, model : Model.Model):
        return self.invalidState(command)

    def shutdown(self, command : Commands.ShutdownCommand, model : Model.Model):
        return self.invalidState(command)

    def abort(self, command : Commands.AbortCommand, model : Model.Model):
        return self.invalidState(command)

    def enable(self, command : Commands.EnableCommand, model : Model.Model):
        return self.invalidState(command)

    def disable(self, command : Commands.DisableCommand, model : Model.Model):
        return self.invalidState(command)

    def standby(self, command : Commands.StandbyCommand, model : Model.Model):
        return self.invalidState(command)

    def exitControl(self, command : Commands.ExitControlCommand, model : Model.Model):
        return self.invalidState(command)

    def start(self, command : Commands.StartCommand, model : Model.Model):
        return self.invalidState(command)

    def enterControl(self, command : Commands.EnterControlCommand, model : Model.Model):
        return self.invalidState(command)

    def setValue(self, command : Commands.SetValueCommand, model : Model.Model):
        return self.invalidState(command)

    def scannerData(self, command : Commands.ScannerDataCommand, model : Model.Model):
        return self.invalidState(command)

    def invalidState(self, command : Commands.Command):
        self.log.error("%s is invalid in the %s state." % (command.name, self.name))
        command.ackInvalidState("Command is not valid in the {state} state.".format(state = self.name))
        return States.NoStateTransition

class OfflineState(State):
    def __init__(self):
        State.__init__(self)

    def boot(self, command : Commands.BootCommand, model : Model.Model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        command.ackComplete()
        return States.StandbyState

class StandbyState(State):
    def __init__(self):
        State.__init__(self)

    def update(self, command : Commands.UpdateCommand, model : Model.Model):
        self.log.debug("Executing %s in %s." % (command.name, self.name))
        model.outerLoop()
        return States.NoStateTransition

    def start(self, command : Commands.StartCommand, model : Model.Model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        command.ackComplete()
        return States.DisabledState

    def shutdown(self, command, model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        model.triggerShutdown()
        command.ackComplete()
        return States.OfflineState    

class DisabledState(State):
    def __init__(self):
        State.__init__(self)

    def update(self, command : Commands.UpdateCommand, model : Model.Model):
        self.log.debug("Executing %s in %s." % (command.name, self.name))
        model.outerLoop()
        return States.NoStateTransition

    def enable(self, command : Commands.EnableCommand, model : Model.Model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        command.ackComplete()
        return States.EnabledState

    def standby(self, command : Commands.StandbyCommand, model : Model.Model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        command.ackComplete()
        return States.StandbyState

    def scannerData(self, command, model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        model.processThermalScannerData(command.getData())
        return States.NoStateTransition

class EnabledState(State):
    def __init__(self):
        State.__init__(self)

    def update(self, command : Commands.UpdateCommand, model : Model.Model):
        self.log.debug("Executing %s in %s." % (command.name, self.name))
        model.outerLoop()
        return States.NoStateTransition

    def disable(self, command : Commands.DisableCommand, model : Model.Model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        command.ackComplete()
        return States.DisabledState

    def scannerData(self, command, model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        model.processThermalScannerData(command.getData())
        return States.NoStateTransition

class FaultState(State):
    def __init__(self):
        State.__init__(self)

    def update(self, command : Commands.UpdateCommand, model : Model.Model):
        self.log.debug("Executing %s in %s." % (command.name, self.name))
        model.outerLoop()
        return States.NoStateTransition

    def standby(self, command : Commands.StandbyCommand, model : Model.Model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        command.ackComplete()
        return States.StandbyState

    def scannerData(self, command, model):
        self.log.info("Executing %s in %s." % (command.name, self.name))
        model.processThermalScannerData(command.getData())
        return States.NoStateTransition