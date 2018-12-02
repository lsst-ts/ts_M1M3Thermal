import logging
import SALPY_MTM1M3TS
import MTM1M3TSController

class Command(object):
    def __init__(self):
        self.name = type(self).__name__
        self.log = logging.getLogger("Command")
        self.log.debug("Creating command %s." % self.name)

    def validate(self):
        return True
    
    def ack(self, ackCode : int, errorCode : int, description : str):
        return SALPY_MTM1M3TS.SAL__OK

    def ackInProgress(self):
        self.log.debug("Sending %s ack command in progress." % self.name)
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_INPROGRESS, 0, "In-Progress")

    def ackComplete(self):
        self.log.info("Sending %s ack command completed." % self.name)
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_COMPLETE, 0, "Complete")

    def ackNotPermitted(self, errorCode : int, description : str):
        self.log.warning("Sending %s ack command not permitted. Error code %d. %s" % (self.name, errorCode, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_NOPERM, errorCode, "Not Permitted: %s" % (description))

    def ackAborted(self, errorCode : int, description : str):
        self.log.warning("Sending %s ack command aborted. Error code %d. %s" % (self.name, errorCode, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_ABORTED, errorCode, "Aborted: %s" % (description))

    def ackFailed(self, errorCode : int, description : str):
        self.log.warning("Sending %s ack command failed. Error code %d. %s" % (self.name, errorCode, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_FAILED, errorCode, "Failed: %s" % (description))

    def ackInvalidState(self, description : str):
        self.log.warning("Sending %s ack command invalid state. %s" % (self.name, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_FAILED, -320, "Failed: %s" % (description))

    def ackInvalidParameter(self, description : str):
        self.log.warning("Sending %s ack command invalid parameter. %s" % (self.name, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_FAILED, -321, "Failed: %s" % (description))

    def ackAlreadyInProgress(self, description : str):
        self.log.warning("Sending %s ack command already in progress. %s" % (self.name, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_FAILED, -322, "Failed: %s" % (description))

    def ackExecutionBlocked(self, description : str):
        self.log.warning("Sending %s ack command execution blocked. %s" % (self.name, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_FAILED, -323, "Failed: %s" % (description))

    def ackAlreadyInState(self, description : str):
        self.log.warning("Sending %s ack command already in state. %s" % (self.name, description))
        return self.ack(SALPY_MTM1M3TS.SAL__CMD_FAILED, -324, "Failed: %s" % (description))

class SALCommand(Command):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data):
        Command.__init__(self)
        self.sal = sal
        self.commandId = commandId
        self.data = data

    def getCommandId(self):
        return self.commandId

    def getData(self):
        return self.data

class BootCommand(Command):
    def __init__(self):
        Command.__init__(self)

class UpdateCommand(Command):
    def __init__(self):
        Command.__init__(self)

class ApplySetpointCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_applySetpointC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_applySetpoint(self.commandId, ackCode, errorCode, description)

class ShutdownCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_shutdownC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_shutdown(self.commandId, ackCode, errorCode, description)

class AbortCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_abortC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_abort(self.commandId, ackCode, errorCode, description)

class EnableCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_enableC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_enable(self.commandId, ackCode, errorCode, description)

class DisableCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_disableC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_disable(self.commandId, ackCode, errorCode, description)

class StandbyCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_standbyC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_standby(self.commandId, ackCode, errorCode, description)

class ExitControlCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_exitControlC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_exitControl(self.commandId, ackCode, errorCode, description)

class StartCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_startC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_start(self.commandId, ackCode, errorCode, description)

class EnterControlCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_enterControlC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_enterControl(self.commandId, ackCode, errorCode, description)

class SetValueCommand(SALCommand):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, commandId : int, data : SALPY_MTM1M3TS.MTM1M3TS_command_setValueC):
        SALCommand.__init__(self, sal, commandId, data)

    def ack(self, ackCode : int, errorCode : int, description : str):
        return self.sal.ackCommand_setValue(self.commandId, ackCode, errorCode, description)

class ScannerDataCommand(Command):
    def __init__(self, data):
        Command.__init__(self)
        self.data = data

    def validate(self):
        if len(self.data) != 4 or len(self.data[0]) != 95 or len(self.data[1]) != 95 or len(self.data[2]) != 95 or len(self.data[3]) != 95:
            self.ackInvalidParameter("Data is invalid")
            return False
        return True

    def getData(self):
        return self.data