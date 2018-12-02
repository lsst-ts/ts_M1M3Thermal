import time
from SALPY_MTM1M3TS import *

class MTM1M3TSRemote:
    def __init__(self, index = 0):
        self.sal = SAL_MTM1M3TS(index)
        self.sal.setDebugLevel(0)
        self.sal.salCommand("MTM1M3TS_command_applySetpoint")
        self.sal.salCommand("MTM1M3TS_command_shutdown")
        self.sal.salCommand("MTM1M3TS_command_abort")
        self.sal.salCommand("MTM1M3TS_command_enable")
        self.sal.salCommand("MTM1M3TS_command_disable")
        self.sal.salCommand("MTM1M3TS_command_standby")
        self.sal.salCommand("MTM1M3TS_command_exitControl")
        self.sal.salCommand("MTM1M3TS_command_start")
        self.sal.salCommand("MTM1M3TS_command_enterControl")
        self.sal.salCommand("MTM1M3TS_command_setValue")

        self.sal.salEvent("MTM1M3TS_logevent_appliedSetpoint")
        self.sal.salEvent("MTM1M3TS_logevent_detailedState")
        self.sal.salEvent("MTM1M3TS_logevent_settingVersions")
        self.sal.salEvent("MTM1M3TS_logevent_errorCode")
        self.sal.salEvent("MTM1M3TS_logevent_summaryState")
        self.sal.salEvent("MTM1M3TS_logevent_appliedSettingsMatchStart")

        self.sal.salTelemetrySub("MTM1M3TS_thermalData")

        self.eventSubscribers_appliedSetpoint = []
        self.eventSubscribers_detailedState = []
        self.eventSubscribers_settingVersions = []
        self.eventSubscribers_errorCode = []
        self.eventSubscribers_summaryState = []
        self.eventSubscribers_appliedSettingsMatchStart = []

        self.telemetrySubscribers_thermalData = []

        self.topicsSubscribedToo = {}

    def close(self):
        time.sleep(1)
        self.sal.salShutdown()

    def flush(self, action):
        result, data = action()
        while result >= 0:
            result, data = action()
            
    def checkForSubscriber(self, action, subscribers):
        buffer = []
        result, data = action()
        while result == 0:
            buffer.append(data)
            result, data = action()
        if len(buffer) > 0:
            for subscriber in subscribers:
                subscriber(buffer)
            
    def runSubscriberChecks(self):
        for subscribedTopic in self.topicsSubscribedToo:
            action = self.topicsSubscribedToo[subscribedTopic][0]
            subscribers = self.topicsSubscribedToo[subscribedTopic][1]
            self.checkForSubscriber(action, subscribers)
            
    def getEvent(self, action):
        lastResult, lastData = action()
        while lastResult >= 0:
            result, data = action()
            if result >= 0:
                lastResult = result
                lastData = data
            elif result < 0:
                break
        return lastResult, lastData

    def getTimestamp(self):
        return self.sal.getCurrentTime()


    def issueCommand_applySetpoint(self, setpoint):
        data = MTM1M3TS_command_applySetpointC()
        data.setpoint = setpoint

        return self.sal.issueCommand_applySetpoint(data)

    def getResponse_applySetpoint(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_applySetpoint(data)
        return result, data
        
    def waitForCompletion_applySetpoint(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_applySetpoint(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_applySetpoint()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_applySetpoint(self, setpoint, timeoutInSeconds = 10):
        cmdId = self.issueCommand_applySetpoint(setpoint)
        return self.waitForCompletion_applySetpoint(cmdId, timeoutInSeconds)

    def issueCommand_shutdown(self, value):
        data = MTM1M3TS_command_shutdownC()
        data.value = value

        return self.sal.issueCommand_shutdown(data)

    def getResponse_shutdown(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_shutdown(data)
        return result, data
        
    def waitForCompletion_shutdown(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_shutdown(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_shutdown()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_shutdown(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_shutdown(value)
        return self.waitForCompletion_shutdown(cmdId, timeoutInSeconds)

    def issueCommand_abort(self, value):
        data = MTM1M3TS_command_abortC()
        data.value = value

        return self.sal.issueCommand_abort(data)

    def getResponse_abort(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_abort(data)
        return result, data
        
    def waitForCompletion_abort(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_abort(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_abort()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_abort(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_abort(value)
        return self.waitForCompletion_abort(cmdId, timeoutInSeconds)

    def issueCommand_enable(self, value):
        data = MTM1M3TS_command_enableC()
        data.value = value

        return self.sal.issueCommand_enable(data)

    def getResponse_enable(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_enable(data)
        return result, data
        
    def waitForCompletion_enable(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_enable(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_enable()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_enable(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_enable(value)
        return self.waitForCompletion_enable(cmdId, timeoutInSeconds)

    def issueCommand_disable(self, value):
        data = MTM1M3TS_command_disableC()
        data.value = value

        return self.sal.issueCommand_disable(data)

    def getResponse_disable(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_disable(data)
        return result, data
        
    def waitForCompletion_disable(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_disable(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_disable()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_disable(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_disable(value)
        return self.waitForCompletion_disable(cmdId, timeoutInSeconds)

    def issueCommand_standby(self, value):
        data = MTM1M3TS_command_standbyC()
        data.value = value

        return self.sal.issueCommand_standby(data)

    def getResponse_standby(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_standby(data)
        return result, data
        
    def waitForCompletion_standby(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_standby(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_standby()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_standby(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_standby(value)
        return self.waitForCompletion_standby(cmdId, timeoutInSeconds)

    def issueCommand_exitControl(self, value):
        data = MTM1M3TS_command_exitControlC()
        data.value = value

        return self.sal.issueCommand_exitControl(data)

    def getResponse_exitControl(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_exitControl(data)
        return result, data
        
    def waitForCompletion_exitControl(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_exitControl(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_exitControl()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_exitControl(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_exitControl(value)
        return self.waitForCompletion_exitControl(cmdId, timeoutInSeconds)

    def issueCommand_start(self, settingsToApply):
        data = MTM1M3TS_command_startC()
        data.settingsToApply = settingsToApply

        return self.sal.issueCommand_start(data)

    def getResponse_start(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_start(data)
        return result, data
        
    def waitForCompletion_start(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_start(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_start()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_start(self, settingsToApply, timeoutInSeconds = 10):
        cmdId = self.issueCommand_start(settingsToApply)
        return self.waitForCompletion_start(cmdId, timeoutInSeconds)

    def issueCommand_enterControl(self, value):
        data = MTM1M3TS_command_enterControlC()
        data.value = value

        return self.sal.issueCommand_enterControl(data)

    def getResponse_enterControl(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_enterControl(data)
        return result, data
        
    def waitForCompletion_enterControl(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_enterControl(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_enterControl()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_enterControl(self, value, timeoutInSeconds = 10):
        cmdId = self.issueCommand_enterControl(value)
        return self.waitForCompletion_enterControl(cmdId, timeoutInSeconds)

    def issueCommand_setValue(self, parametersAndValues):
        data = MTM1M3TS_command_setValueC()
        data.parametersAndValues = parametersAndValues

        return self.sal.issueCommand_setValue(data)

    def getResponse_setValue(self):
        data = MTM1M3TS_ackcmdC()
        result = self.sal.getResponse_setValue(data)
        return result, data
        
    def waitForCompletion_setValue(self, cmdId, timeoutInSeconds = 10):
        waitResult = self.sal.waitForCompletion_setValue(cmdId, timeoutInSeconds)
        #ackResult, ack = self.getResponse_setValue()
        #return waitResult, ackResult, ack
        return waitResult
        
    def issueCommandThenWait_setValue(self, parametersAndValues, timeoutInSeconds = 10):
        cmdId = self.issueCommand_setValue(parametersAndValues)
        return self.waitForCompletion_setValue(cmdId, timeoutInSeconds)



    def getNextEvent_appliedSetpoint(self):
        data = MTM1M3TS_logevent_appliedSetpointC()
        result = self.sal.getEvent_appliedSetpoint(data)
        return result, data
        
    def getEvent_appliedSetpoint(self):
        return self.getEvent(self.getNextEvent_appliedSetpoint)
        
    def subscribeEvent_appliedSetpoint(self, action):
        self.eventSubscribers_appliedSetpoint.append(action)
        if "event_appliedSetpoint" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["event_appliedSetpoint"] = [self.getNextEvent_appliedSetpoint, self.eventSubscribers_appliedSetpoint]

    def getNextEvent_detailedState(self):
        data = MTM1M3TS_logevent_detailedStateC()
        result = self.sal.getEvent_detailedState(data)
        return result, data
        
    def getEvent_detailedState(self):
        return self.getEvent(self.getNextEvent_detailedState)
        
    def subscribeEvent_detailedState(self, action):
        self.eventSubscribers_detailedState.append(action)
        if "event_detailedState" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["event_detailedState"] = [self.getNextEvent_detailedState, self.eventSubscribers_detailedState]

    def getNextEvent_settingVersions(self):
        data = MTM1M3TS_logevent_settingVersionsC()
        result = self.sal.getEvent_settingVersions(data)
        return result, data
        
    def getEvent_settingVersions(self):
        return self.getEvent(self.getNextEvent_settingVersions)
        
    def subscribeEvent_settingVersions(self, action):
        self.eventSubscribers_settingVersions.append(action)
        if "event_settingVersions" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["event_settingVersions"] = [self.getNextEvent_settingVersions, self.eventSubscribers_settingVersions]

    def getNextEvent_errorCode(self):
        data = MTM1M3TS_logevent_errorCodeC()
        result = self.sal.getEvent_errorCode(data)
        return result, data
        
    def getEvent_errorCode(self):
        return self.getEvent(self.getNextEvent_errorCode)
        
    def subscribeEvent_errorCode(self, action):
        self.eventSubscribers_errorCode.append(action)
        if "event_errorCode" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["event_errorCode"] = [self.getNextEvent_errorCode, self.eventSubscribers_errorCode]

    def getNextEvent_summaryState(self):
        data = MTM1M3TS_logevent_summaryStateC()
        result = self.sal.getEvent_summaryState(data)
        return result, data
        
    def getEvent_summaryState(self):
        return self.getEvent(self.getNextEvent_summaryState)
        
    def subscribeEvent_summaryState(self, action):
        self.eventSubscribers_summaryState.append(action)
        if "event_summaryState" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["event_summaryState"] = [self.getNextEvent_summaryState, self.eventSubscribers_summaryState]

    def getNextEvent_appliedSettingsMatchStart(self):
        data = MTM1M3TS_logevent_appliedSettingsMatchStartC()
        result = self.sal.getEvent_appliedSettingsMatchStart(data)
        return result, data
        
    def getEvent_appliedSettingsMatchStart(self):
        return self.getEvent(self.getNextEvent_appliedSettingsMatchStart)
        
    def subscribeEvent_appliedSettingsMatchStart(self, action):
        self.eventSubscribers_appliedSettingsMatchStart.append(action)
        if "event_appliedSettingsMatchStart" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["event_appliedSettingsMatchStart"] = [self.getNextEvent_appliedSettingsMatchStart, self.eventSubscribers_appliedSettingsMatchStart]



    def getNextSample_thermalData(self):
        data = MTM1M3TS_thermalDataC()
        result = self.sal.getNextSample_thermalData(data)
        return result, data

    def getSample_thermalData(self):
        data = MTM1M3TS_thermalDataC()
        result = self.sal.getSample_thermalData(data)
        return result, data
        
    def subscribeTelemetry_thermalData(self, action):
        self.telemetrySubscribers_thermalData.append(action)
        if "telemetry_thermalData" not in self.topicsSubscribedToo:
            self.topicsSubscribedToo["telemetry_thermalData"] = [self.getNextSample_thermalData, self.telemetrySubscribers_thermalData]

