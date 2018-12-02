import logging
import MTM1M3TSController
import queue
import time

class Model(object):
    def __init__(self, sal : MTM1M3TSController.MTM1M3TSController):
        self.log = logging.getLogger("Model")
        self.sal = sal
        self.shutdown = queue.Queue()
        self.lastOuterLoop = 0

    def getSAL(self):
        return self.sal

    def waitForShutdown(self):
        self.log.info("Waiting for shutdown.")
        self.shutdown.get()

    def triggerShutdown(self):
        self.log.info("Triggering shutdown.")
        self.shutdown.put(0)

    def outerLoop(self):
        current = time.time()
        self.log.debug("Outer loop execution time %0.4f" % (current - self.lastOuterLoop))
        self.lastOuterLoop = current

    def processThermalScannerData(self, data):
        self.log.debug("Process thermal scanner data")
        self.sal.putSample_thermalData(self.sal.getTimestamp(), data[0], data[1], data[2], data[3])