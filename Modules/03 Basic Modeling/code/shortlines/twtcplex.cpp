#include <tuple>
#include <map>
#include <sstream>
#include <ilcplex/ilocplex.h>
typedef std::map<std::tuple<IloInt,IloInt>,IloBoolVar> IloBoolVarPairMap;

int main(int argc, char* argv[])
{
    IloEnv env;
    try
    {
        // TWT Problem Data
        const IloInt nbJobs = 4;
        const IloNumArray weight(env, nbJobs, 4, 5, 3, 5);
        const IloNumArray duration(env, nbJobs, 12, 8, 15, 9);
        const IloNumArray deadline(env, nbJobs, 16, 26, 25, 27);
        const IloNum M = IloSum(duration);

        // Create a new model
        IloModel model(env, "TWTexample");

        // Create variables
        // x(i)(j) = 1 if i << j, else j >> i
        IloBoolVarPairMap x;
        for (IloInt i = 0; i < nbJobs; ++i)
        {
            for (IloInt j = i+1; j < nbJobs; ++j)
            {
                std::ostringstream varname;
                varname << "x(" << i << ")(" << j << ")";
                x.emplace(std::make_tuple(i, j), IloBoolVar(env, varname.str().c_str()));
            }
        }
        IloNumVarArray startTime(env, nbJobs, 0.0, IloInfinity);
        startTime.setNames("startTime");
        IloNumVarArray tardiness(env, nbJobs, 0.0, IloInfinity);
        tardiness.setNames("tardiness");

        // Set objective function
        model.add(IloMinimize(env, IloScalProd(weight, tardiness) ));

        // Add constraints
        for (auto& kv : x)
        {
            IloInt i, j;
            IloBoolVar x_ij;
            std::forward_as_tuple(std::tie(i, j), x_ij) = kv;

            IloConstraint ovCons1(startTime[j] >= startTime[i] + duration[i] - M*(1-x_ij));
            std::ostringstream consname;
            consname << "NoOverlap1_" << i << "_"  << j;
            ovCons1.setName(consname.str().c_str());
            model.add(ovCons1);

            IloConstraint ovCons2(startTime[i] >= startTime[j] + duration[j] - M*x_ij);
            consname.str("");
            consname << "NoOverlap2_" << i << "_"  << j;
            ovCons2.setName(consname.str().c_str());
            model.add(ovCons2);
        }
        for (IloInt i = 0; i < nbJobs; i++)
        {
            IloConstraint ddlCons(tardiness[i] >= startTime[i] + duration[i] - deadline[i]);
            std::ostringstream consname;
            consname << "Deadline_" << i;
            ddlCons.setName(consname.str().c_str());
            model.add(ddlCons);
        }

        // Solve model
        IloCplex cplex(model);
        cplex.solve();
        if (cplex.getStatus() == IloAlgorithm::InfeasibleOrUnbounded)
        {
            cplex.setParam(IloCplex::Param::Preprocessing::Reduce, 0);
            cplex.solve();
        }

        // Display solution
        if (cplex.getStatus() == IloAlgorithm::Optimal)
        {
            for (IloIterator<IloNumVar> it(env); it.ok(); ++it)
            {
                IloNumVar var = *it;
                if (cplex.isExtracted(var))
                {
                    std::string varname = var.getName();
                    double varval = cplex.getValue(var);
                    env.out() << varname << ":\t" << varval << std::endl;
                }
            }
            env.out() << "Objective:\t" << cplex.getObjValue() << std::endl;
        }
        else
        {
            IloAlgorithm::Status status = cplex.getStatus();
            env.out() << "Optimization was stopped with status " << status << std::endl;
        }
    }
    catch (const IloException& ex)
    {
        std::cerr << "CPLEX Error: " << ex << std::endl;
    }
    catch (...)
    {
        std::cerr << "Error" << std::endl;
    }
    env.end();
    return 0;
}
