#!/usr/bin/python3
from docplex.mp.model import Model
from docloud.status import JobSolveStatus
from cplex.exceptions import CplexSolverError

def TWTcombined():
    # TWT Problem Data
    jobs = tuple([i+1 for i in range(4)])
    weight = dict(zip(jobs, (4, 5, 3, 5)))
    duration = dict(zip(jobs, (12, 8, 15, 9)))
    deadline = dict(zip(jobs, (16, 26, 25, 27)))
    M = sum(duration.values())

    jobPairs = [(i,j) for i in jobs for j in jobs if i < j]

    # Create a new model
    with Model(name='TWTexample', log_output=True) as m:

        # Create variables
        # x_i_j = 1 if i << j, else j >> i
        x = m.binary_var_dict(jobPairs, name='x')
        S = m.continuous_var_dict(jobs, name='S')
        T = m.continuous_var_dict(jobs, name='T')

        # Set objective function
        m.minimize(m.sum(weight[j]*T[j] for j in jobs))

        # Add constraints
        m.add_constraints(
            (S[j] >= S[i] + duration[i] - M*(1-x[(i,j)]),
                'NoOverlap1[%d,%d]' % (i,j))
            for (i,j) in jobPairs)
        m.add_constraints(
            (S[i] >= S[j] + duration[j] - M*x[(i,j)],
                'NoOverlap2[%d,%d]' % (i,j))
            for (i,j) in jobPairs)
        m.add_constraints(
            (T[j] >= S[j] + duration[j] - deadline[j],
                'Deadline[%d]' % j)
            for j in jobs)

        try:
            c = m.get_cplex()
            c.solve()
            cstatus = c.solution.get_status()

            if cstatus == c.solution.status.MIP_infeasible_or_unbounded:
                # Disable primal/dual reductions to determine solve status
                c.parameters.preprocessing.reduce.set(0)
                c.solve()
                cstatus = c.solution.get_status()

            if cstatus == c.solution.status.MIP_optimal:
                for i in range(c.variables.get_num()):
                    vname = c.variables.get_names(i)
                    vval = c.solution.get_values(i)
                    print('%s:\t%g' % (vname, vval))
                objval = c.solution.get_objective_value()
                print('Obj:\t%g' % objval)
            else:
                statstr = c.solution.get_status_string()
                print('Optimization was stopped with status %s' % str(statstr))

        except CplexSolverError as e:
            print('Exception raised during solve: %s' % str(e))

if __name__ == '__main__':
    TWTcombined()
