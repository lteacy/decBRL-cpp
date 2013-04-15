#!/usr/bin/env python
import ProtoMDP_pb2
import sys
from numpy import *

mdp = ProtoMDP_pb2.FactoredMDP()
mdp.name = "Simple Test MDP"
mdp.description = "My first factored MDP in protocol buffers"
mdp.gamma = 0.9

for stateId in range(1,3):
   state = mdp.states.add()
   state.id = stateId
   state.size = stateId+1

for actionId in range(3,5):
   action = mdp.actions.add()
   action.id = actionId
   action.size = actionId-1

for rewardId in range(1,3):
   reward = mdp.rewards.add()
   reward.id = rewardId
   states = [mdp.states[rewardId-1], mdp.actions[rewardId-1] ]
   sizes = [s.size for s in states]
   reward.domain.extend([s.id for s in states])
   domainSize = 1;
   for siz in sizes:
      domainSize *= siz

   reward.values.extend(range(0,domainSize))
   reward.std_dev.extend([1]*domainSize)

for state in mdp.states:
   cpt = mdp.transitions.add()
   cpt.domain.append(state.id)
   cpt.conditions.extend([s.id for s in mdp.states])
   domainSize = state.size
   condSize = 1;
   for state in mdp.states:
      condSize *= state.size;

   transProb = arange(0,domainSize*condSize,dtype=double)
   transProb = transProb.reshape([domainSize,condSize])
   transProb /= transProb.sum(axis=0)

   print transProb

   cpt.values.extend(transProb.transpose().flatten().tolist())

print mdp

# Write mdp to file
f = open(sys.argv[1], "wb")
f.write(mdp.SerializeToString())
f.close()

