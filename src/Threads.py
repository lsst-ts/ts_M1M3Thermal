import logging
import threading
import MTM1M3TSController
import Controller
import Commands
import time

class Thread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self, name = type(self).__name__.replace("Thread", ""))
        self.log = logging.getLogger("Thread")
        self.running = False

    def stop(self):
        self.log.info("Stopping thread %s." % self.name)
        self.running = False

class SubscriberThread(Thread):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController, controller : Controller.Controller, loopTimeInSec : float):
        Thread.__init__(self)
        self.sal = sal
        self.controller = controller
        self.loopTimeInSec = loopTimeInSec

        self.sal.subscribeCommand_applySetpoint(lambda commandId, data: self.controller.enqueue(Commands.ApplySetpointCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_shutdown(lambda commandId, data: self.controller.enqueue(Commands.ShutdownCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_abort(lambda commandId, data: self.controller.enqueue(Commands.AbortCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_enable(lambda commandId, data: self.controller.enqueue(Commands.EnableCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_disable(lambda commandId, data: self.controller.enqueue(Commands.DisableCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_standby(lambda commandId, data: self.controller.enqueue(Commands.StandbyCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_exitControl(lambda commandId, data: self.controller.enqueue(Commands.ExitControlCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_start(lambda commandId, data: self.controller.enqueue(Commands.StartCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_enterControl(lambda commandId, data: self.controller.enqueue(Commands.EnterControlCommand(self.sal, commandId, data)))
        self.sal.subscribeCommand_setValue(lambda commandId, data: self.controller.enqueue(Commands.SetValueCommand(self.sal, commandId, data)))

    def run(self):
        self.log.info("Starting thread %s." % self.name)
        self.running = True
        while self.running:
            self.sal.runSubscriberChecks()
            time.sleep(self.loopTimeInSec)
        self.log.info("Thread %s completed." % self.name)

class ControllerThread(Thread):
    def __init__(self, controller : Controller.Controller, loopTimeInSec : float):
        Thread.__init__(self)
        self.controller = controller
        self.loopTimeInSec = loopTimeInSec

    def run(self):
        self.log.info("Starting thread %s." % self.name)
        self.running = True
        while self.running:
            command = self.controller.dequeue()
            self.controller.execute(command)
            time.sleep(self.loopTimeInSec)
        self.log.info("Thread %s completed." % self.name)

class OuterLoopThread(Thread):
    def __init__(self, controller : Controller.Controller, loopTimeInSec : float):
        Thread.__init__(self)
        self.controller = controller
        self.loopTimeInSec = loopTimeInSec

    def run(self):
        self.log.info("Starting thread %s." % self.name)
        self.running = True
        while self.running:
            self.controller.enqueue(Commands.UpdateCommand())
            time.sleep(self.loopTimeInSec)
        self.log.info("Thread %s completed." % self.name)

class ThermalScannerThread(Thread):
    def __init__(self, controller, scanner1, scanner2, scanner3, scanner4):
        Thread.__init__(self)
        self.controller = controller
        self.scanner1 = scanner1
        self.scanner2 = scanner2
        self.scanner3 = scanner3
        self.scanner4 = scanner4

    def run(self):
        self.log.info("Starting thread %s" % self.name)
        self.running = True
        while self.running:
            scanner1Data = self.scanner1.receive()
            scanner2Data = self.scanner2.receive()
            scanner3Data = self.scanner3.receive()
            scanner4Data = self.scanner4.receive()
            if self.running:
                self.controller.enqueue(Commands.ScannerDataCommand([scanner1Data, scanner2Data, scanner3Data, scanner4Data]))
        self.log.info("Thread %s completed" % self.name)