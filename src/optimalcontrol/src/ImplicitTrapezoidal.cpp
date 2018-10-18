/*
 * Copyright (C) 2014,2018 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 *
 * Originally developed for Prioritized Optimal Control (2014)
 * Refactored in 2018.
 * Design inspired by
 * - ACADO toolbox (http://acado.github.io)
 * - ADRL Control Toolbox (https://adrlab.bitbucket.io/ct/ct_doc/doc/html/index.html)
 */

#include <iDynTree/Integrators/ImplicitTrapezoidal.h>
#include <iDynTree/DynamicalSystem.h>
#include <iDynTree/Core/Utils.h>
#include <Eigen/Dense>
#include <iDynTree/Core/EigenHelpers.h>
#include <cstddef>
#include <sstream>
#include <string>

namespace iDynTree {
    namespace optimalcontrol {
        namespace integrators {

            bool ImplicitTrapezoidal::allocateBuffers()
            {
                if (!m_dynamicalSystem_ptr) {
                    return false;
                }

                m_computationBuffer.resize(static_cast<unsigned int>(m_dynamicalSystem_ptr->stateSpaceSize()));
                m_computationBuffer2.resize(static_cast<unsigned int>(m_dynamicalSystem_ptr->stateSpaceSize()));

                unsigned int nx = static_cast<unsigned int>(m_dynamicalSystem_ptr->stateSpaceSize());
                unsigned int nu = static_cast<unsigned int>(m_dynamicalSystem_ptr->controlSpaceSize());

                m_identity.resize(nx, nx);
                toEigen(m_identity) = iDynTreeEigenMatrix::Identity(nx, nx);

                m_stateJacBuffer.resize(nx, nx);
                m_controlJacBuffer.resize(nx,nu);

                m_stateJacobianSparsity.resize(2);
                m_controlJacobianSparsity.resize(2);

                if (m_dynamicalSystem_ptr->dynamicsStateFirstDerivativeSparsity(m_stateJacobianSparsity[0])) {

                    for (size_t i = 0; i < m_dynamicalSystem_ptr->stateSpaceSize(); ++i) {
                        m_stateJacobianSparsity[0].addNonZeroIfNotPresent(i, i);
                    }
                    m_stateJacobianSparsity[1] = m_stateJacobianSparsity[0];

                    m_hasStateSparsity = true;
                }

                if (m_dynamicalSystem_ptr->dynamicsControlFirstDerivativeSparsity(m_controlJacobianSparsity[0])) {

                    m_controlJacobianSparsity[1] = m_controlJacobianSparsity[0];

                    m_hasControlSparsity = true;

                }

                return true;
            }

            bool ImplicitTrapezoidal::oneStepIntegration(double t0, double dT, const iDynTree::VectorDynSize &x0, iDynTree::VectorDynSize &x)
            {
                reportError(m_info.name().c_str(), "oneStepIntegration", "The ImplicitTrapezoidal method has not been implemented to integrate a dynamical system yet.");
                return false;
            }

            ImplicitTrapezoidal::ImplicitTrapezoidal()
            {
                m_infoData->name = "ImplicitTrapezoidal";
                m_infoData->isExplicit = false;
                m_infoData->numberOfStages = 1;
            }

            ImplicitTrapezoidal::ImplicitTrapezoidal(const std::shared_ptr<DynamicalSystem> dynamicalSystem) : FixedStepIntegrator(dynamicalSystem)
            {
                m_infoData->name = "ImplicitTrapezoidal";
                m_infoData->isExplicit = false;
                m_infoData->numberOfStages = 1;
                allocateBuffers();
            }

            ImplicitTrapezoidal::~ImplicitTrapezoidal()
            {}

            bool ImplicitTrapezoidal::evaluateCollocationConstraint(double time, const std::vector<VectorDynSize> &collocationPoints,
                                                                    const std::vector<VectorDynSize> &controlInputs, double dT,
                                                                    VectorDynSize &constraintValue)
            {
                if (!m_dynamicalSystem_ptr){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Dynamical system not set.");
                    return false;
                }

                if (collocationPoints.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the collocation point does not match the expected one. Input = ";
                    errorMsg << collocationPoints.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", errorMsg.str().c_str());
                    return false;
                }

                if (controlInputs.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the control inputs does not match the expected one. Input = ";
                    errorMsg << controlInputs.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", errorMsg.str().c_str());
                    return false;
                }

                if (constraintValue.size() != m_dynamicalSystem_ptr->stateSpaceSize()) {
                    constraintValue.resize(static_cast<unsigned int>(m_dynamicalSystem_ptr->stateSpaceSize()));
                }

                if (!((m_dynamicalSystem_ptr->setControlInput(controlInputs[0])))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Error while setting the control input.");
                    return false;
                }

                if (!(m_dynamicalSystem_ptr->dynamics(collocationPoints[0], time, m_computationBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Error while evaluating the dynamical system.");
                    return false;
                }

                if (!((m_dynamicalSystem_ptr->setControlInput(controlInputs[1])))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Error while setting the control input.");
                    return false;
                }

                if (!(m_dynamicalSystem_ptr->dynamics(collocationPoints[1], time+dT, m_computationBuffer2))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Error while evaluating the dynamical system.");
                    return false;
                }

                toEigen(constraintValue) = -toEigen(collocationPoints[1]) + toEigen(collocationPoints[0]) +
                        dT/2.0*(toEigen(m_computationBuffer)+toEigen(m_computationBuffer2));

                return true;
            }

            bool ImplicitTrapezoidal::evaluateCollocationConstraintJacobian(double time, const std::vector<VectorDynSize> &collocationPoints,
                                                                            const std::vector<VectorDynSize> &controlInputs, double dT,
                                                                            std::vector<MatrixDynSize> &stateJacobianValues,
                                                                            std::vector<MatrixDynSize> &controlJacobianValues)
            {
                if (!m_dynamicalSystem_ptr){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Dynamical system not set.");
                    return false;
                }

                if (collocationPoints.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the collocation point does not match the expected one. Input = ";
                    errorMsg << collocationPoints.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian", errorMsg.str().c_str());
                    return false;
                }

                if (controlInputs.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the control inputs does not match the expected one. Input = ";
                    errorMsg << controlInputs.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian", errorMsg.str().c_str());
                    return false;
                }

                unsigned int nx = static_cast<unsigned int>(m_dynamicalSystem_ptr->stateSpaceSize());
                unsigned int nu = static_cast<unsigned int>(m_dynamicalSystem_ptr->controlSpaceSize());

                if (stateJacobianValues.size() != 2) {
                    stateJacobianValues.resize(2);
                }

                if (controlJacobianValues.size() != 2) {
                    controlJacobianValues.resize(2);
                }

                if ((stateJacobianValues[0].rows() != nx) || (stateJacobianValues[0].cols() != nx)) {
                    stateJacobianValues[0].resize(nx,nx);
                }

                if (!((m_dynamicalSystem_ptr->setControlInput(controlInputs[0])))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian", "Error while setting the control input.");
                    return false;
                }

                if (!(m_dynamicalSystem_ptr->dynamicsStateFirstDerivative(collocationPoints[0], time, m_stateJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system jacobian.");
                    return false;
                }

                toEigen(stateJacobianValues[0]) = toEigen(m_identity) + dT/2*toEigen(m_stateJacBuffer);


                if ((controlJacobianValues[0].rows() != nx) || (controlJacobianValues[0].cols() != nu)) {
                    controlJacobianValues[0].resize(nx,nu);
                }


                if (!(m_dynamicalSystem_ptr->dynamicsControlFirstDerivative(collocationPoints[0], time, m_controlJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system control jacobian.");
                    return false;
                }

                toEigen(controlJacobianValues[0]) = dT/2.0*toEigen(m_controlJacBuffer);

                if ((stateJacobianValues[1].rows() != nx) || (stateJacobianValues[1].cols() != nx)) {
                    stateJacobianValues[1].resize(nx,nx);
                }

                if (!((m_dynamicalSystem_ptr->setControlInput(controlInputs[1])))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian", "Error while setting the control input.");
                    return false;
                }

                if (!(m_dynamicalSystem_ptr->dynamicsStateFirstDerivative(collocationPoints[1], time+dT, m_stateJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system jacobian.");
                    return false;
                }

                toEigen(stateJacobianValues[1]) = -toEigen(m_identity) + dT/2*toEigen(m_stateJacBuffer);


                if ((controlJacobianValues[1].rows() != nx) || (controlJacobianValues[1].cols() != nu)) {
                    controlJacobianValues[1].resize(nx,nu);
                }

                if (!(m_dynamicalSystem_ptr->dynamicsControlFirstDerivative(collocationPoints[1], time + dT, m_controlJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system control jacobian.");
                    return false;
                }

                toEigen(controlJacobianValues[1]) = dT/2.0*toEigen(m_controlJacBuffer);

                return true;

            }

            bool ImplicitTrapezoidal::getCollocationConstraintJacobianStateSparsity(std::vector<SparsityStructure> &stateJacobianSparsity)
            {
                if (!m_hasStateSparsity) {
                    return false;
                }

                stateJacobianSparsity = m_stateJacobianSparsity;
                return true;
            }

            bool ImplicitTrapezoidal::getCollocationConstraintJacobianControlSparsity(std::vector<SparsityStructure> &controlJacobianSparsity)
            {
                if (!m_hasControlSparsity) {
                    return false;
                }

                controlJacobianSparsity = m_controlJacobianSparsity;
                return true;
            }

        }
    }
}
