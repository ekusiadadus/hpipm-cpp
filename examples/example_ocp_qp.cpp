#include "hpipm-cpp/ocp_qp_dim.hpp"
#include "hpipm-cpp/ocp_qp.hpp"
#include "hpipm-cpp/ocp_qp_ipm_arg.hpp"
#include "hpipm-cpp/ocp_qp_ipm.hpp"

#include "getting_started_data.hpp"

#include <iostream>
#include <vector>
#include "Eigen/Core"


int main() {
  QPSize qp_size;
  hpipm::ocp_qp_dim dim;
  dim.N    = qp_size.N;
  dim.nx   = qp_size.nx;
  dim.nu   = qp_size.nu;
  dim.nbx  = qp_size.nbx;
  dim.nbu  = qp_size.nbu;
  dim.ng   = qp_size.ng;
  dim.nsbx = qp_size.nsbx;
  dim.nsbu = qp_size.nsbu;
  dim.nsg  = qp_size.nsg;
  dim.create_hpipm();

  const auto dim_err_msg = dim.checkSize();
  if (!dim_err_msg.empty()) {
    for (const auto& e : dim_err_msg) {
      std::cout << e << std::endl;
    }
  }

  QPData qp_data;
  hpipm::ocp_qp qp;
  for (int i=0; i<dim.N; ++i) {
    qp.A.push_back(qp_data.A);
    qp.B.push_back(qp_data.B);
    qp.b.push_back(qp_data.b);
  }
  for (int i=0; i<=dim.N; ++i) {
    qp.Q.push_back(qp_data.Q);
    qp.R.push_back(qp_data.R);
    qp.S.push_back(qp_data.S);
    qp.q.push_back(qp_data.q);
    qp.r.push_back(qp_data.r);
  }
  qp.idxbx.push_back(qp_data.idxbx0);
  qp.lbx.push_back(qp_data.lbx0);
  qp.ubx.push_back(qp_data.ubx0);
  for (int i=1; i<=dim.N; ++i) {
    qp.idxbx.push_back(qp_data.idxbx);
    qp.lbx.push_back(qp_data.lbx);
    qp.ubx.push_back(qp_data.ubx);
  }
  qp.create_hpipm(dim);
  const auto qp_err_msg = qp.checkSize(dim);
  if (!qp_err_msg.empty()) {
    for (const auto& e : qp_err_msg) {
      std::cout << e << std::endl;
    }
  }

  IPMArg ipm_arg;
  hpipm::ocp_qp_ipm_arg arg;
  arg.mode = static_cast<hpipm::HpipmMode>(ipm_arg.mode);
  arg.iter_max = ipm_arg.iter_max;
  arg.alpha_min = ipm_arg.alpha_min;
  arg.mu0 = ipm_arg.mu0;
  arg.tol_stat = ipm_arg.tol_stat;
  arg.tol_eq = ipm_arg.tol_eq;
  arg.tol_ineq = ipm_arg.tol_ineq;
  arg.tol_comp = ipm_arg.tol_comp;
  arg.reg_prim = ipm_arg.reg_prim;
  arg.warm_start = ipm_arg.warm_start;
  arg.pred_corr = ipm_arg.pred_corr;
  arg.ric_alg = ipm_arg.ric_alg;
  arg.split_step = ipm_arg.split_step;
  arg.create_hpipm(dim);

  hpipm::ocp_qp_sol sol;
  sol.create_hpipm(dim);
  const auto sol_err_msg = sol.checkSize(dim);
  if (!sol_err_msg.empty()) {
    for (const auto& e : sol_err_msg) {
      std::cout << e << std::endl;
    }
  }

  hpipm::ocp_qp_ipm ipm;
  ipm.create_hpipm(dim, arg);
  const auto res = ipm.solve(qp, sol, arg);
  std::cout << "QP result: " << res << std::endl;

  sol.from_hpipm(dim);

  std::cout << "OCP QP primal solution: " << std::endl;
  for (int i=0; i<=dim.N; ++i) {
    std::cout << "x[" << i << "]: " << sol.x[i].transpose() << std::endl;
  }
  for (int i=0; i<dim.N; ++i) {
    std::cout << "u[" << i << "]: " << sol.u[i].transpose() << std::endl;
  }

  std::cout << "OCP QP dual solution (Lagrange multipliers): " << std::endl;
  for (int i=0; i<dim.N; ++i) {
    std::cout << "pi[" << i << "]: " << sol.pi[i].transpose() << std::endl;
  }

  return 0;
}