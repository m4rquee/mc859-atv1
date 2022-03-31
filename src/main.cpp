#include <algorithm>
#include <gurobi_c++.h>
#include <iostream>
#include <iterator>

#define LIN2D(i, j, W) (i * W + j)
#define LIN3D(i, j, k, W, H) (k * W * H + j * W + i)

short J, F, L, M, P;
std::vector<int> D, r, R, C, p_cost, t_cost;

void print_solution(GRBVar *X, GRBVar *Y) {
    std::cout << "Solution:" << std::endl;
    for (int f = 0; f < F; f++) {
        std::cout << std::endl
                  << "-Factory " << f << ":" << std::endl;
        for (int p = 0; p < P; p++) {
            std::cout << "  -Product " << p << ":" << std::endl;
            for (int l = 0; l < L; l++)
                std::cout << "      make " << X[LIN3D(p, l, f, P, L)].get(GRB_DoubleAttr_X)
                          << " tons in the machine " << l << std::endl;

            for (int j = 0; j < J; j++)
                std::cout << "      send " << Y[LIN3D(p, f, j, P, F)].get(GRB_DoubleAttr_X)
                          << " tons to the client " << j << std::endl;
        }
    }
}

void read_data() {
    std::cin >> J >> F >> L >> M >> P;
    std::copy_n(std::istream_iterator<int>(std::cin), J * P, std::back_inserter(D));
    std::copy_n(std::istream_iterator<int>(std::cin), M * P * L, std::back_inserter(r));
    std::copy_n(std::istream_iterator<int>(std::cin), M * F, std::back_inserter(R));
    std::copy_n(std::istream_iterator<int>(std::cin), L * F, std::back_inserter(C));
    std::copy_n(std::istream_iterator<int>(std::cin), P * L * F, std::back_inserter(p_cost));
    std::copy_n(std::istream_iterator<int>(std::cin), P * F * J, std::back_inserter(t_cost));
}

int main() {
    read_data();
    auto X = new GRBVar[P * L * F];
    auto Y = new GRBVar[P * F * J];

    try {
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);
        model.set(GRB_StringAttr_ModelName, "Atividade 1");
        model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

        // Quantity, in tons, of the product p to be made in the machine l of the factory f:
        for (int p = 0; p < P; p++)
            for (int l = 0; l < L; l++)
                for (int f = 0; f < F; f++) {
                    auto name = "X_" + std::to_string(p) + std::to_string(l) + std::to_string(f);
                    X[LIN3D(p, l, f, P, L)] = model.addVar(0.0, GRB_INFINITY, p_cost[LIN3D(p, l, f, P, L)], GRB_CONTINUOUS, name);
                }

        // Quantity, in tons, of the product p to be sent from the factory f to the client j:
        for (int p = 0; p < P; p++)
            for (int f = 0; f < F; f++)
                for (int j = 0; j < J; j++) {
                    auto name = "Y_" + std::to_string(p) + std::to_string(f) + std::to_string(j);
                    Y[LIN3D(p, f, j, P, F)] = model.addVar(0.0, GRB_INFINITY, t_cost[LIN3D(p, f, j, P, F)], GRB_CONTINUOUS, name);
                }

        model.update();// run update to use model inserted variables

        // Demand restriction of client j for the product p:
        for (int j = 0; j < J; j++)
            for (int p = 0; p < P; p++) {
                GRBLinExpr expr = 0;
                for (int f = 0; f < F; f++)
                    expr += Y[LIN3D(p, f, j, P, F)];
                model.addConstr(expr == D[LIN2D(j, p, P)]);
            }

        // Use restriction of the raw material m by all L machines of the f factory:
        for (int m = 0; m < M; m++)
            for (int f = 0; f < F; f++) {
                GRBLinExpr expr = 0;
                for (int p = 0; p < P; p++)
                    for (int l = 0; l < L; l++)
                        expr += X[LIN3D(p, l, f, P, L)] * r[LIN3D(m, p, l, M, P)];
                model.addConstr(expr <= R[LIN2D(m, f, F)]);
            }

        // Capacity restriction of each machine l of each f factory:
        for (int l = 0; l < L; l++)
            for (int f = 0; f < F; f++) {
                GRBLinExpr expr = 0;
                for (int p = 0; p < P; p++)
                    expr += X[LIN3D(p, l, f, P, L)];
                model.addConstr(expr <= C[LIN2D(l, f, F)]);
            }

        // Product production equals its transportation on each factory:
        for (int p = 0; p < P; p++)
            for (int f = 0; f < F; f++) {
                GRBLinExpr exprA = 0, exprB = 0;
                for (int l = 0; l < L; l++)// total produced by all machines
                    exprA += X[LIN3D(p, l, f, P, L)];
                for (int j = 0; j < J; j++)// total transported to all clients
                    exprB += Y[LIN3D(p, f, j, P, F)];
                model.addConstr(exprA == exprB);
            }


        model.optimize();
        if (model.get(GRB_IntAttr_SolCount) == 0)// if the solver could not obtain a solution
            throw GRBException("Could not obtain a solution!", -1);

        print_solution(X, Y);
    } catch (const GRBException &ex) {
        printf("Exception...\n");
        std::cout << ex.getMessage();
        exit(ex.getErrorCode());
    }
    delete[] X;
    delete[] Y;
    return 0;
}
