#!/usr/bin/python3
from cplex import Cplex, SparsePair
from cplex.exceptions import CplexSolverError

def TWTcplex():
    # TWT Problem Data
    jobs = tuple([i+1 for i in range(4)])
    weight = dict(zip(jobs, (4, 5, 3, 5)))
    duration = dict(zip(jobs, (12, 8, 15, 9)))
    deadline = dict(zip(jobs, (16, 26, 25, 27)))
    M = sum(duration.values())

    jobPairs = [(i,j) for i in jobs for j in jobs if i < j]

    try:
        # Create a new model
        m = Cplex()
        m.set_problem_name('TWTexample')

        # Create variables
        # x_{i,j} = 1 if i << j, else j >> i
        x = {(i,j): 'x_{%d,%d}' % (i,j) for (i,j) in jobPairs}
        S = {j: 'S_%d' % j for j in jobs}
        T = {j: 'T_%d' % j for j in jobs}
        m.variables.add(
            types=['B' for p in jobPairs],
            names=[x[p] for p in jobPairs]
        )
        m.variables.add(names=[S[j] for j in jobs])
        m.variables.add(names=[T[j] for j in jobs])

        # Set objective function
        m.objective.set_linear([(T[j], weight[j]) for j in jobs])
        m.objective.set_sense(m.objective.sense.minimize)

        # Add constraints
        m.linear_constraints.add(
            lin_expr=[
                SparsePair([x[(i,j)], S[i], S[j]], [-M, -1, 1])
                for (i,j) in jobPairs
            ],
            senses=['G' for p in jobPairs],
            rhs=[duration[i] - M for (i,j) in jobPairs],
            names=['NoOverlap1_%d%d' % (i,j) for (i,j) in jobPairs]
        )
        m.linear_constraints.add(
            lin_expr=[
                SparsePair([x[(i,j)], S[i], S[j]], [M, 1, -1])
                for (i,j) in jobPairs
            ],
            senses=['G' for p in jobPairs],
            rhs=[duration[j] for (i,j) in jobPairs],
            names=['NoOverlap2_%d%d' % (i,j) for (i,j) in jobPairs]
        )
        m.linear_constraints.add(
            lin_expr=[
                SparsePair([S[j], T[j]], [-1, 1])
                for j in jobs
            ],
            senses=['G' for j in jobs],
            rhs=[duration[j] - deadline[j] for j in jobs],
            names=['Deadline_%d' % j for j in jobs]
        )

        m.solve()
        mstatus = m.solution.get_status()

        if mstatus == m.solution.status.MIP_infeasible_or_unbounded:
            # Disable primal/dual reductions to determine solve status
            m.parameters.preprocessing.reduce.set(0)
            m.solve()
            mstatus = m.solution.get_status()

        if mstatus == m.solution.status.MIP_optimal:
            for i in range(m.variables.get_num()):
                vname = m.variables.get_names(i)
                vval = m.solution.get_values(i)
                print('%s:\t%g' % (vname, vval))
            objval = m.solution.get_objective_value()
            print('Obj:\t%g' % objval)
        else:
            statstr = m.solution.get_status_string()
            print('Optimization was stopped with status %s' % str(statstr))

    except CplexSolverError as e:
        print('Exception raised during solve: %s' % str(e))

if __name__ == '__main__':
    TWTcplex()
