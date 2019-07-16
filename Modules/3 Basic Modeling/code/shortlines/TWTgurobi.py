#!/usr/bin/python3
from gurobipy import Model, GRB, GurobiError, quicksum
StatusDict = {getattr(GRB.Status, s): s for s in dir(GRB.Status) if s.isupper()}

def TWTgurobi():
    # TWT Problem Data
    jobs = tuple([i+1 for i in range(4)])
    jobPairs = [(i,j) for i in jobs for j in jobs if i < j]
    weight = dict(zip(jobs, (4, 5, 3, 5)))
    duration = dict(zip(jobs, (12, 8, 15, 9)))
    deadline = dict(zip(jobs, (16, 26, 25, 27)))
    M = sum(duration.values())

    try:
        # Create a new model
        m = Model('TWTexample')

        # Create variables
        # x[(i,j)] = 1 if i << j, else j >> i
        x = m.addVars(jobPairs, vtype=GRB.BINARY, name='x')
        startTime = m.addVars(jobs, name='startTime')
        tardiness = m.addVars(jobs, name='tardiness')

        # Set objective function
        m.setObjective(quicksum([weight[j]*tardiness[j] for j in jobs]),
            GRB.MINIMIZE)

        # Add constraints
        m.addConstrs(
            (startTime[j] >= startTime[i] + duration[i] - M*(1-x[(i,j)])
                for (i,j) in jobPairs), 'NoOverlap1')
        m.addConstrs(
            (startTime[i] >= startTime[j] + duration[j] - M*x[(i,j)]
                for (i,j) in jobPairs), 'NoOverlap2')
        m.addConstrs(
            (tardiness[j] >= startTime[j] + duration[j] - deadline[j]
                for j in jobs), 'Deadline')

        # Solve model
        m.optimize()
        if m.status == GRB.Status.INF_OR_UNBD:
            # Disable dual reductions to determine solve status
            m.setParam(GRB.Param.DualReductions, 0)
            m.optimize()

        # Display solution
        if m.status == GRB.Status.OPTIMAL:
            for v in m.getVars():
                print('%s:\t%g' % (v.varName, v.x))
            print('Objective:\t%g' % m.objVal)
        else:
            statstr = StatusDict[m.status]
            print('Optimization was stopped with status %s' % statstr)

    except GurobiError as e:
        print('Error code ' + str(e.errno) + ": " + str(e))

if __name__ == '__main__':
    TWTgurobi()
