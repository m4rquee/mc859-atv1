#include <gurobi_c++.h>
#include <iostream>

int main() {
    const short F = 3, J = 3, L = 3, M = 3, P = 3;
    const double D[J][P] = {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
    const double r[M][P][L] = {
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}};
    const double R[M][F] = {{100, 100, 100}, {100, 100, 100}, {100, 100, 100}};
    const double C[L][F] = {{100, 100, 100}, {100, 100, 100}, {100, 100, 100}};
    const double p_cost[P][L][F] = {
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}};
    const double t_cost[P][F][J] = {
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
            {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}};

    GRBVar X[P][L][F];
    GRBVar Y[P][F][J];
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.set(GRB_StringAttr_ModelName, "Atividade 1");
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

    // Quantity, in tons, of the product p to be made in the machine l of the factory f:
    for (int p = 0; p < P; p++)
        for (int l = 0; l < L; l++)
            for (int f = 0; f < F; f++) {
                std::string name = "X_" + std::to_string(p) + std::to_string(l) + std::to_string(f);
                X[p][l][f] = model.addVar(0.0, GRB_INFINITY, p_cost[p][l][f], GRB_CONTINUOUS, name);
            }

    // Quantity, in tons, of the product p to be sent from the factory f to the client j:
    for (int p = 0; p < P; p++)
        for (int f = 0; f < F; f++)
            for (int j = 0; j < J; j++) {
                std::string name = "Y_" + std::to_string(p) + std::to_string(f) + std::to_string(j);
                Y[p][f][j] = model.addVar(0.0, GRB_INFINITY, t_cost[p][f][j], GRB_CONTINUOUS, name);
            }

    model.update();// run update to use model inserted variables

    // Demand restriction of client j for the product p:
    for (int j = 0; j < J; j++)
        for (int p = 0; p < P; p++) {
            GRBLinExpr expr = 0;
            for (int f = 0; f < F; f++)
                expr += Y[p][f][j];
            model.addConstr(expr == D[j][p]);
        }

    // Use restriction of the raw material m by all L machines of the f factory:
    for (int m = 0; m < M; m++)
        for (int f = 0; f < F; f++) {
            GRBLinExpr expr = 0;
            for (int p = 0; p < P; p++)
                for (int l = 0; l < L; l++)
                    expr += X[p][l][f] * r[m][p][l];
            model.addConstr(expr <= R[m][f]);
        }

    // Capacity restriction of each machine l of each f factory:
    for (int l = 0; l < L; l++)
        for (int f = 0; f < F; f++) {
            GRBLinExpr expr = 0;
            for (int p = 0; p < P; p++)
                expr += X[p][l][f];
            model.addConstr(expr <= C[l][f]);
        }

    // Product production equals its transportation on each factory:
    for (int p = 0; p < P; p++)
        for (int f = 0; f < F; f++) {
            GRBLinExpr exprA = 0, exprB = 0;
            for (int l = 0; l < L; l++)// total produced by all machines
                exprA += X[p][l][f];
            for (int j = 0; j < J; j++)// total transported to all clients
                exprB += Y[p][f][j];
            model.addConstr(exprA == exprB);
        }

    try {
        model.optimize();
        if (model.get(GRB_IntAttr_SolCount) == 0)// if the solver could not obtain a solution
            throw "Could not obtain a solution!";


        std::cout << "Solution:" << std::endl;
        for (int f = 0; f < F; f++) {
            std::cout << std::endl
                      << "-Factory " << f << ":" << std::endl;
            for (int p = 0; p < P; p++) {
                std::cout << "  -Product " << p << ":" << std::endl;
                for (int l = 0; l < L; l++)
                    std::cout << "      make " << X[p][l][f].get(GRB_DoubleAttr_X)
                              << " tons in the machine " << l << std::endl;

                for (int j = 0; j < J; j++)
                    std::cout << "      send " << Y[p][f][j].get(GRB_DoubleAttr_X)
                              << " tons to the client " << j << std::endl;
            }
        }
    } catch (...) {
        printf("Exception...\n");
        exit(1);
    }
    return 0;
}
