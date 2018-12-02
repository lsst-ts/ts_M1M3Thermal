import sys
import os
import threading
import unittest
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src"))
import Main
import MTM1M3TSRemote
import MTM1M3TSEnumerations
import time
import socket

class MTM1M3TS(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self, name = "App")
        
    def run(self):
        Main.run()

class MTM1M3TSTests(unittest.TestCase):
    def setUp(self):
        self.app = MTM1M3TS()
        self.app.start()
        self.remote = MTM1M3TSRemote.MTM1M3TSRemote()
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.StandbyState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.StandbyState, "Should be in StandbyState")
        
    def tearDown(self):
        self.remote.issueCommand_shutdown(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.OfflineState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.OfflineState, "Should be in OfflineState")
        self.remote.close()
        self.app.join()

    def test_stateMachine(self):
        self.remote.issueCommand_start("Default")
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.DisabledState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.DisabledState, "Should be in DisabledState")
        self.remote.issueCommand_enable(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.EnabledState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.EnabledState, "Should be in EnabledState")
        self.remote.issueCommand_disable(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.DisabledState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.DisabledState, "Should be in DisabledState")
        self.remote.issueCommand_standby(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.StandbyState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.StandbyState, "Should be in StandbyState")

    def test_thermalData(self):
        udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.remote.issueCommand_start("Default")
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.DisabledState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.DisabledState, "Should be in DisabledState")
        udp.sendto(",".join([str(x) for x in ([1] * 96)]).encode("utf-8"), ("127.0.0.1", 10001))
        udp.sendto(",".join([str(x) for x in ([2] * 96)]).encode("utf-8"), ("127.0.0.1", 10002))
        udp.sendto(",".join([str(x) for x in ([3] * 96)]).encode("utf-8"), ("127.0.0.1", 10003))
        udp.sendto(",".join([str(x) for x in ([4] * 96)]).encode("utf-8"), ("127.0.0.1", 10004))
        time.sleep(1)
        result, data = self.remote.getSample_thermalData()
        self.assertEqual(1, data.thermocoupleScanner1[0], "Thermal scanner 1 in DisabledState")
        self.assertEqual(2, data.thermocoupleScanner2[0], "Thermal scanner 2 in DisabledState")
        self.assertEqual(3, data.thermocoupleScanner3[0], "Thermal scanner 3 in DisabledState")
        self.assertEqual(4, data.thermocoupleScanner4[0], "Thermal scanner 4 in DisabledState")
        self.remote.issueCommand_enable(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.EnabledState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.EnabledState, "Should be in EnabledState")
        udp.sendto(",".join([str(x) for x in ([5] * 96)]).encode("utf-8"), ("127.0.0.1", 10001))
        udp.sendto(",".join([str(x) for x in ([6] * 96)]).encode("utf-8"), ("127.0.0.1", 10002))
        udp.sendto(",".join([str(x) for x in ([7] * 96)]).encode("utf-8"), ("127.0.0.1", 10003))
        udp.sendto(",".join([str(x) for x in ([8] * 96)]).encode("utf-8"), ("127.0.0.1", 10004))
        time.sleep(1)
        result, data = self.remote.getSample_thermalData()
        self.assertEqual(5, data.thermocoupleScanner1[94], "Thermal scanner 1 in EnabledState")
        self.assertEqual(6, data.thermocoupleScanner2[94], "Thermal scanner 2 in EnabledState")
        self.assertEqual(7, data.thermocoupleScanner3[94], "Thermal scanner 3 in EnabledState")
        self.assertEqual(8, data.thermocoupleScanner4[94], "Thermal scanner 4 in EnabledState")
        self.remote.issueCommand_disable(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.DisabledState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.DisabledState, "Should be in DisabledState")
        self.remote.issueCommand_standby(False)
        data = self.waitUntil(self.remote.getNextEvent_summaryState, lambda x: x.summaryState == MTM1M3TSEnumerations.SummaryStates.StandbyState)
        self.assertEqual(data.summaryState, MTM1M3TSEnumerations.SummaryStates.StandbyState, "Should be in StandbyState")
        udp.close()

    def waitUntil(self, action, predicate, timeout = 5):
        startTime = time.time()
        result, data = action()
        while not predicate(data) and (time.time() - startTime) < timeout:
            time.sleep(1)
            result, data = action()
        return data

if __name__ == '__main__':
    unittest.main()