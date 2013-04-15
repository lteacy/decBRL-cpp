#!/usr/bin/env python
import ProtoMDP_pb2
import sys

mdp = ProtoMDP_pb2.FactoredMDP()
# Read the existing mdp.
f = open(sys.argv[1], "rb")
mdp.ParseFromString(f.read())
f.close()

print mdp

