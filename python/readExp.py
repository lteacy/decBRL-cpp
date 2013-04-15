#!/usr/bin/env python
import Experiment_pb2
import sys

exp = Experiment_pb2.ExperimentSetup()
# Read the existing mdp.
f = open(sys.argv[1], "rb")
exp.ParseFromString(f.read())
f.close()

print exp

