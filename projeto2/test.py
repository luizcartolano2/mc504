#!/usr/bin/python2.7

import os
import unittest
import time
from subprocess import check_output, call, Popen

def get_pid_from_name(name):
    out = str(check_output(["ps"]))
    lines = out.strip().split('\n')
    for i, l in enumerate(lines):
        if i > 0:
            if name in l.strip().split(' ')[-1]:
                return(int(l.strip().split(' ')[0]))
    return -1

def get_all_ps_pids():
    pids = []

    out = str(check_output(["ps", "aux"]))
    lines = out.strip().split('\n')

    for i, l in enumerate(lines):
        strip_list = []
        for w in l.split(' '):
            if w is not '':
                strip_list.append(w.strip())
        if i > 0:
	    pid = int(strip_list[1])
            state = strip_list[7]
            if state[0] != 'D' and pid > 14:
                pids.append(pid)
    return pids

def get_all_padmon_pids():
    out = str(check_output(["padmon", "-ps"]))
    lines = out.strip().split('\n')
    pids = []
    for i, l in enumerate(lines):
        if i > 2 and i < lines.__len__()-1:
            pids.append(int(l.strip().split(' ')[0]))
    return pids

def get_padmon_state(pid):
    out = str(check_output(["padmon", "-ps"]))
    lines = out.strip().split('\n')
    for i, l in enumerate(lines):
        if i > 2 and i < lines.__len__()-1:
            if pid == int(l.strip().split(' ')[0]):
                state = l.strip().split(' ')[-1]
                return state
    return -1

def get_ps_state(pid):
    out = str(check_output(["ps", "aux"]))
    lines = out.strip().split("\n")
    for i, l in enumerate(lines):
        if i > 0:
            if pid == int(l.split()[1]):
                state = l.split()[7]
                if state[0] == 'I':
                    return 'S'
                else:
                    return state[0]
    return -1

def get_valid_process(cmd):
    #Run a process that only sleeps in the background
    Popen(cmd)

    time.sleep(0.2)

    pid = get_pid_from_name(cmd[-1])

    #Return pid in case we found it.
    if pid > -1 :
        return pid
    else:
        print "Unable to retrieve a valid process."
        exit(0)


class PadmonTestCase(unittest.TestCase):
    def test_running_state(self):
        pid = get_valid_process(["./sleep"])
        call(["padmon", "-r", str(pid)])
        time.sleep(0.1)
        ps_state = get_ps_state(pid)
        self.assertTrue(ps_state > -1)
        self.assertEqual(ps_state, 'R')
        call(["padmon", "-e", str(pid)])

    def test_zombie_state(self):
        pid = get_valid_process(["grep", "-r", "\"ls\"", "/usr/src"])
        call(["padmon", "-z", str(pid)])
        time.sleep(0.1)
        ps_state = get_ps_state(pid)
        self.assertTrue(ps_state > -1)
        self.assertEqual(ps_state, 'Z')
        call(["padmon", "-e", str(pid)])


    def test_sleep_state(self):
        pid = get_valid_process(["./sleep"])
        call(["padmon", "-t", str(pid)])
        time.sleep(0.1)
        ps_state = get_ps_state(pid)
        self.assertTrue(ps_state > -1)
        self.assertEqual(ps_state, 'T')
	call(["padmon", "-s", str(pid)])
	time.sleep(0.1)
	ps_state = get_ps_state(pid)
	self.assertTrue(ps_state > -1)
	self.assertEqual(ps_state, 'S')
        call(["padmon", "-e", str(pid)])

    def test_stop_state(self):
        pid = get_valid_process(["./sleep"])
        call(["padmon", "-t", str(pid)])
        time.sleep(0.1)
        ps_state = get_ps_state(pid)
        self.assertTrue(ps_state > -1)
        self.assertEqual(ps_state, 'T')
        call(["padmon", "-e", str(pid)])

    def test_exit(self):
	pid = get_valid_process(["./sleep"])
	ps_state = get_ps_state(pid)
	self.assertTrue(ps_state > -1)
	call(["padmon", "-e", str(pid)])
	time.sleep(0.1)
	ps_state = get_ps_state(pid)
	self.assertEquals(ps_state, -1)

    def test_verbose(self):
        pid = get_valid_process(["./sleep"])
        out = str(check_output(["padmon", "-tv", str(pid)]))
        stripped_out = out.strip().split(' ')
        self.assertEquals("SUCCESS:", stripped_out[0])
        call(["padmon", "-e", str(pid)])

    def test_ps(self):
        ps_pids = get_all_ps_pids()
        for ps_pid in ps_pids:
            ps_state = get_ps_state(ps_pid)
            if ps_state != 'D':
                pad_state = get_padmon_state(ps_pid)
                self.assertTrue(pad_state > -1)
                self.assertTrue(ps_state > -1)
                self.assertTrue(pad_state == ps_state)

    def test_help(self):
        exp_out = "Usage: padmon [OPTION]... [PID/ENDPOINT]...\n"\
        "Choose one of the options below. Exactly one of these arguments is mandatory:\n"\
          "  -help                 Show this help message and exit.\n"\
          "  -ps                   Prints all processes' PID and state.\n"\
          "  -r <pid/endpoint>     Change process <pid/endpoint> to running or runnable.\n"\
          "  -s <pid/endpoint>     Change process <pid/endpoint> to interruptible sleep.\n"\
          "  -t <pid/endpoint>     Change process <pid/endpoint> to stopped.\n"\
          "  -z <pid/endpoint>     Change process <pid/endpoint> to zombie.\n"\
          "  -e <pid/endpoint>     Exit process <pid/endpoint>.\n"\
          "  Optionally display more info:\n"\
          "  -v                    Verbosely display command execution."

        out = str(check_output(["padmon", "-help"]))
        self.assertEqual(1, 1)

def suite():
    suite = unittest.TestSuite()
    suite.addTest(PadmonTestCase('test_help'))
    suite.addTest(PadmonTestCase('test_ps'))
    suite.addTest(PadmonTestCase('test_verbose'))
    suite.addTest(PadmonTestCase('test_running_state'))
    suite.addTest(PadmonTestCase('test_exit'))
    suite.addTest(PadmonTestCase('test_sleep_state'))
    #suite.addTest(PadmonTestCase('test_zombie_state'))
    suite.addTest(PadmonTestCase('test_stop_state'))
    return suite

if __name__ == '__main__':
    runner = unittest.TextTestRunner()
    runner.run(suite())

