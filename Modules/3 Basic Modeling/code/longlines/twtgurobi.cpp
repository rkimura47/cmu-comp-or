#include <tuple>
#include <map>
#include <memory>
#include <array>
#include <vector>
#include <sstream>
#include "gurobi_c++.h"
typedef std::map<std::tuple<int,int>,GRBVar> GRBVarPairMap;
typedef std::unique_ptr<GRBVar[]> GRBVarArray;

int main(int argc, char* argv[])
{
    try
    {
        // TWT Problem Data
        const int nbJobs = 4;
        const std::array<double, nbJobs> weight = {4, 5, 3, 5};
        const std::array<double, nbJobs> duration = {12, 8, 15, 9};
        const std::array<double, nbJobs> deadline = {16, 26, 25, 27};
        double M = 0;
        for (int j = 0; j < nbJobs; ++j)
        {
            M += duration[j];
        }

        // Create a new model
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "TWTexample");

        // Create variables
        // x(i)(j) = 1 if i << j, else j >> i
        GRBVarPairMap x;
        for (int i = 0; i < nbJobs; ++i)
        {
            for (int j = i+1; j < nbJobs; ++j)
            {
                std::ostringstream varname;
                varname << "x(" << i << ")(" << j << ")";
                x.emplace(std::make_tuple(i, j), model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varname.str()));
            }
        }
        std::vector<GRBVar> startTime;
        startTime.reserve(nbJobs);
        for (int j = 0; j < nbJobs; ++j)
        {
            std::ostringstream varname;
            varname << "startTime(" << j << ")";
            startTime.emplace_back(model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, varname.str()));
        }
        std::vector<GRBVar> tardiness;
        tardiness.reserve(nbJobs);
        for (int j = 0; j < nbJobs; ++j)
        {
            std::ostringstream varname;
            varname << "tardiness(" << j << ")";
            tardiness.emplace_back(model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, varname.str()));
        }

        // Set objective function
        GRBLinExpr obj = 0;
        for (int j = 0; j  < nbJobs; ++j)
        {
            obj += weight[j]*tardiness[j];
        }
        model.setObjective(obj, GRB_MINIMIZE);

        // Add constraints
        for (auto& kv : x)
        {
            int i, j;
            GRBVar x_ij;
            std::forward_as_tuple(std::tie(i, j), x_ij) = kv;

            std::ostringstream consname;
            consname << "NoOverlap1_" << i << "_"  << j;
            model.addConstr(startTime[j] >= startTime[i] + duration[i] - M*(1-x_ij), consname.str());
            consname.str("");
            consname << "NoOverlap2_" << i << "_" << j;
            model.addConstr(startTime[i] >= startTime[j] + duration[j] - M*x_ij, consname.str());
        }
        for (int i = 0; i < nbJobs; ++i)
        {
            std::ostringstream consname;
            consname << "Deadline_" << i;
            model.addConstr(tardiness[i] >= startTime[i] + duration[i] - deadline[i], consname.str());
        }

        // Solve model
        model.optimize();
        if (model.get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
        {
            model.set(GRB_IntParam_DualReductions, 0);
            model.optimize();
        }

        // Display solution
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL)
        {
            int numVars = model.get(GRB_IntAttr_NumVars);
            auto varArray = GRBVarArray(model.getVars());
            for (int i = 0; i < numVars; ++i)
            {
                std::string varname = varArray[i].get(GRB_StringAttr_VarName);
                double varval = varArray[i].get(GRB_DoubleAttr_X);
                std::cout << varname << ":\t" << varval << std::endl;
            }
            std::cout << "Objective:\t" << model.get(GRB_DoubleAttr_ObjVal) << std::endl;
        }
        else
        {
            int status = model.get(GRB_IntAttr_Status);
            std::cout << "Optimization was stopped with status " << status << std::endl;
        }
    }
    catch (const GRBException& e)
    {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    catch (...)
    {
        std::cout << "Exception during optimization" << std::endl;
    }
    return 0;
}
