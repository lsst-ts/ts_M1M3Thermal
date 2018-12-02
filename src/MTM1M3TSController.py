import time
from SALPY_MTM1M3TS import *

class MTM1M3TSController:
    def __init__(self, index = 0):
        self.sal = SAL_MTM1M3TS(index)
        self.sal.setDebugLevel(0)
        self.sal.salProcessor("MTM1M3TS_command_applySetpoint")
        self.sal.salProcessor("MTM1M3TS_command_shutdown")
        self.sal.salProcessor("MTM1M3TS_command_abort")
        self.sal.salProcessor("MTM1M3TS_command_enable")
        self.sal.salProcessor("MTM1M3TS_command_disable")
        self.sal.salProcessor("MTM1M3TS_command_standby")
        self.sal.salProcessor("MTM1M3TS_command_exitControl")
        self.sal.salProcessor("MTM1M3TS_command_start")
        self.sal.salProcessor("MTM1M3TS_command_enterControl")
        self.sal.salProcessor("MTM1M3TS_command_setValue")

        self.sal.salEventPub("MTM1M3TS_logevent_appliedSetpoint")
        self.sal.salEventPub("MTM1M3TS_logevent_detailedState")
        self.sal.salEventPub("MTM1M3TS_logevent_settingVersions")
        self.sal.salEventPub("MTM1M3TS_logevent_errorCode")
        self.sal.salEventPub("MTM1M3TS_logevent_summaryState")
        self.sal.salEventPub("MTM1M3TS_logevent_appliedSettingsMatchStart")

        self.sal.salTelemetryPub("MTM1M3TS_thermalData")

        self.commandSubscribers_applySetpoint = []
        self.commandSubscribers_shutdown = []
        self.commandSubscribers_abort = []
        self.commandSubscribers_enable = []
        self.commandSubscribers_disable = []
        self.commandSubscribers_standby = []
        self.commandSubscribers_exitControl = []
        self.commandSubscribers_start = []
        self.commandSubscribers_enterControl = []
        self.commandSubscribers_setValue = []

        self.topicsSubscribedToo = {}

    def close(self):
        time.sleep(1)
        self.sal.salShutdown()

    def flush(self, action):
        result, data = action()
        while result >= 0:
            result, data = action()
            
    def checkForSubscriber(self, action, subscribers):
        result, data = action()
        if result > 0:
            for subscriber in subscribers:
                subscriber(result, data)
            
    def runSubscriberChecks(self):
        for subscribedTopic in self.topicsSubscribedToo:
            action = self.topicsSubscribedToo[subscribedTopic][0]
            subscribers = self.topicsSubscribedToo[subscribedTopic][1]
            self.checkForSubscriber(action, subscribers)

    def getTimestamp(self):
        return self.sal.getCurrentTime()


    def acceptCommand_applySetpoint(self):
        data = MTM1M3TS_command_applySetpointC()
        result = self.sal.acceptCommand_applySetpoint(data)
        return result, data

    def ackCommand_applySetpoint(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_applySetpoint(cmdId, ackCode, errorCode, description)

    def subscribeCommand_applySetpoint(self, action):
        self.commandSubscribers_applySetpoint.append(action)
        if "command_applySetpoint" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_applySetpoint"] = [self.acceptCommand_applySetpoint, self.commandSubscribers_applySetpoint]

    def acceptCommand_shutdown(self):
        data = MTM1M3TS_command_shutdownC()
        result = self.sal.acceptCommand_shutdown(data)
        return result, data

    def ackCommand_shutdown(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_shutdown(cmdId, ackCode, errorCode, description)

    def subscribeCommand_shutdown(self, action):
        self.commandSubscribers_shutdown.append(action)
        if "command_shutdown" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_shutdown"] = [self.acceptCommand_shutdown, self.commandSubscribers_shutdown]

    def acceptCommand_abort(self):
        data = MTM1M3TS_command_abortC()
        result = self.sal.acceptCommand_abort(data)
        return result, data

    def ackCommand_abort(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_abort(cmdId, ackCode, errorCode, description)

    def subscribeCommand_abort(self, action):
        self.commandSubscribers_abort.append(action)
        if "command_abort" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_abort"] = [self.acceptCommand_abort, self.commandSubscribers_abort]

    def acceptCommand_enable(self):
        data = MTM1M3TS_command_enableC()
        result = self.sal.acceptCommand_enable(data)
        return result, data

    def ackCommand_enable(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_enable(cmdId, ackCode, errorCode, description)

    def subscribeCommand_enable(self, action):
        self.commandSubscribers_enable.append(action)
        if "command_enable" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_enable"] = [self.acceptCommand_enable, self.commandSubscribers_enable]

    def acceptCommand_disable(self):
        data = MTM1M3TS_command_disableC()
        result = self.sal.acceptCommand_disable(data)
        return result, data

    def ackCommand_disable(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_disable(cmdId, ackCode, errorCode, description)

    def subscribeCommand_disable(self, action):
        self.commandSubscribers_disable.append(action)
        if "command_disable" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_disable"] = [self.acceptCommand_disable, self.commandSubscribers_disable]

    def acceptCommand_standby(self):
        data = MTM1M3TS_command_standbyC()
        result = self.sal.acceptCommand_standby(data)
        return result, data

    def ackCommand_standby(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_standby(cmdId, ackCode, errorCode, description)

    def subscribeCommand_standby(self, action):
        self.commandSubscribers_standby.append(action)
        if "command_standby" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_standby"] = [self.acceptCommand_standby, self.commandSubscribers_standby]

    def acceptCommand_exitControl(self):
        data = MTM1M3TS_command_exitControlC()
        result = self.sal.acceptCommand_exitControl(data)
        return result, data

    def ackCommand_exitControl(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_exitControl(cmdId, ackCode, errorCode, description)

    def subscribeCommand_exitControl(self, action):
        self.commandSubscribers_exitControl.append(action)
        if "command_exitControl" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_exitControl"] = [self.acceptCommand_exitControl, self.commandSubscribers_exitControl]

    def acceptCommand_start(self):
        data = MTM1M3TS_command_startC()
        result = self.sal.acceptCommand_start(data)
        return result, data

    def ackCommand_start(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_start(cmdId, ackCode, errorCode, description)

    def subscribeCommand_start(self, action):
        self.commandSubscribers_start.append(action)
        if "command_start" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_start"] = [self.acceptCommand_start, self.commandSubscribers_start]

    def acceptCommand_enterControl(self):
        data = MTM1M3TS_command_enterControlC()
        result = self.sal.acceptCommand_enterControl(data)
        return result, data

    def ackCommand_enterControl(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_enterControl(cmdId, ackCode, errorCode, description)

    def subscribeCommand_enterControl(self, action):
        self.commandSubscribers_enterControl.append(action)
        if "command_enterControl" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_enterControl"] = [self.acceptCommand_enterControl, self.commandSubscribers_enterControl]

    def acceptCommand_setValue(self):
        data = MTM1M3TS_command_setValueC()
        result = self.sal.acceptCommand_setValue(data)
        return result, data

    def ackCommand_setValue(self, cmdId, ackCode, errorCode, description):
        return self.sal.ackCommand_setValue(cmdId, ackCode, errorCode, description)

    def subscribeCommand_setValue(self, action):
        self.commandSubscribers_setValue.append(action)
        if "command_setValue" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["command_setValue"] = [self.acceptCommand_setValue, self.commandSubscribers_setValue]



    def logEvent_appliedSetpoint(self, setpoint, priority = 0):
        data = MTM1M3TS_logevent_appliedSetpointC()
        data.setpoint = setpoint

        return self.sal.logEvent_appliedSetpoint(data, priority)

    def logEvent_detailedState(self, detailedState, priority = 0):
        data = MTM1M3TS_logevent_detailedStateC()
        data.detailedState = detailedState

        return self.sal.logEvent_detailedState(data, priority)

    def logEvent_settingVersions(self, recommendedSettingsVersion, recommendedSettingsLabels, priority = 0):
        data = MTM1M3TS_logevent_settingVersionsC()
        data.recommendedSettingsVersion = recommendedSettingsVersion
        data.recommendedSettingsLabels = recommendedSettingsLabels

        return self.sal.logEvent_settingVersions(data, priority)

    def logEvent_errorCode(self, errorCode, errorReport, traceback, priority = 0):
        data = MTM1M3TS_logevent_errorCodeC()
        data.errorCode = errorCode
        data.errorReport = errorReport
        data.traceback = traceback

        return self.sal.logEvent_errorCode(data, priority)

    def logEvent_summaryState(self, summaryState, priority = 0):
        data = MTM1M3TS_logevent_summaryStateC()
        data.summaryState = summaryState

        return self.sal.logEvent_summaryState(data, priority)

    def logEvent_appliedSettingsMatchStart(self, appliedSettingsMatchStartIsTrue, priority = 0):
        data = MTM1M3TS_logevent_appliedSettingsMatchStartC()
        data.appliedSettingsMatchStartIsTrue = appliedSettingsMatchStartIsTrue

        return self.sal.logEvent_appliedSettingsMatchStart(data, priority)



    def putSample_thermalData(self, timestamp, thermocoupleScanner1, thermocoupleScanner2, thermocoupleScanner3, thermocoupleScanner4):
        data = MTM1M3TS_thermalDataC()
        data.timestamp = timestamp
        for i in range(95):
            data.thermocoupleScanner1[i] = thermocoupleScanner1[i]
        for i in range(95):
            data.thermocoupleScanner2[i] = thermocoupleScanner2[i]
        for i in range(95):
            data.thermocoupleScanner3[i] = thermocoupleScanner3[i]
        for i in range(95):
            data.thermocoupleScanner4[i] = thermocoupleScanner4[i]

        return self.sal.putSample_thermalData(data)

