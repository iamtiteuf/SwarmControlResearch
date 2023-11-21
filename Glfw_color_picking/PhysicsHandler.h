#ifndef PHYSICS_HANDLER_H
#define PHYSICS_HANDLER_H

#include "Glew_Initialization.h"

class PhysicsHandler
{

	public:
		
		
		
		static void LQR_Solver(Eigen::MatrixXd& K,float n, float m,int iter)
		{
			K = LQR(n, m, 1);

			if (K(0, 0) == INFINITY || isnan(K(0, 0)))
			{
				K = LQR(n, m, powf(10, iter));

			}
			/*for (int i = iter; i>= -iter; i--)
			{
				if (K(0, 0) == INFINITY || isnan(K(0, 0)))
				{
					K = LQR(n, m, powf(10,i));
					
				}
				else
				{
					break;
				}
		
			}*/
			if (K(0, 0) == INFINITY || isnan(K(0, 0)))
			{
				K.setIdentity();
			}

			//std::cout << " K " << "\n" << K << "\n" << "\n";
		}
	
		static Eigen::MatrixXd LQR(float n, float m,float scl)
		{
			Eigen::MatrixXd A;
			A.resize(6, 6);
			A << 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 1,
				3 * n * n, 0, 0, 0, 2 * n, 0,
				0, 0, 0, -2 * n, 0, 0,
				0, 0, -n * n, 0, 0, 0;
			/*A << 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 1,
				3 * n * n, 0, 0, 0, 2 * n, 0,
				0, 0, 0, -2 * n, 0, 0,
				0, 0, -n * n, 0, 0, 0;*/
			Eigen::MatrixXd B;
			B.resize(6, 3);
			B << 0, 0, 0, 
				 0, 0,0,
				 0, 0, 0, 
				 1 / m, 0, 0,
				 0, 1 / m, 0,
				 0, 0, 1 / m;

			Eigen::MatrixXd Q;
			Q.resize(6, 6);
			Q << 1, 0, 0, 0, 0, 0,
				 0, 1, 0, 0, 0, 0,
				 0, 0, 1, 0, 0, 0,
				 0, 0, 0, 1, 0, 0,
				 0, 0, 0, 0, 1, 0,
				 0, 0, 0, 0, 0, 1;

			Q = Q * scl;

			Eigen::MatrixXd R;
			R.resize(3, 3);
			R << 1, 0, 0,
				 0, 1, 0,
				 0, 0, 1;

			Eigen::MatrixXd HM;
			HM.resize(2 * A.rows(), 2 * A.cols());

			
			
			HM << A, -B * R.inverse() * B.transpose(),
				-Q, -A.transpose();

			
			Eigen::EigenSolver<Eigen::MatrixXd> solver(HM);
			Eigen::MatrixXd eigenvectors = solver.eigenvectors().real();
			Eigen::VectorXcd eigenvalues = solver.eigenvalues();

			// Create a vector of pairs, where each pair contains an eigenvalue and its corresponding eigenvector
			std::vector<std::pair<std::complex<double>, Eigen::VectorXd>> eigenPairs;
			for (int i = 0; i < eigenvalues.size(); ++i)
			{
				if (eigenvalues[i].real() < 0)
				{
					eigenPairs.push_back(std::make_pair(eigenvalues[i], eigenvectors.col(i).real()));
				}
			}

			// Sort the vector of pairs based on the eigenvalues
			std::sort(eigenPairs.begin(), eigenPairs.end(),
				[](const std::pair<std::complex<double>, Eigen::VectorXd>& a,
					const std::pair<std::complex<double>, Eigen::VectorXd>& b)
				{
					return a.first.real() < b.first.real();
				});

			// Create a matrix of eigenvectors sorted according to their eigenvalues
			Eigen::MatrixXd sortedEigenvectors(eigenvectors.rows(), eigenPairs.size());
			for (int i = 0; i < eigenPairs.size(); ++i)
			{
				sortedEigenvectors.col(i) = eigenPairs[i].second;
			}

		

			//Eigen::MatrixXd eigenvectorsMatrix(eigenvectors.rows(), 0); // Initialize an empty matrix

			//for (int i = 0; i < eigenvalues.size(); ++i) 
			//{
			//	if (eigenvalues[i].real() < 0) { // If the real part of the eigenvalue is negative
			//		eigenvectorsMatrix.conservativeResize(Eigen::NoChange, eigenvectorsMatrix.cols() + 1); // Resize the matrix to add a new column
			//		eigenvectorsMatrix.col(eigenvectorsMatrix.cols() - 1) = eigenvectors.col(i); // Add the eigenvector to the matrix
			//	}
			//}

			//std::cout << "Eigenvectors matrix:\n" << eigenvectorsMatrix << std::endl;

			Eigen::MatrixXd X;
			Eigen::MatrixXd Y;

			int halfRows = sortedEigenvectors.rows() / 2;

			X = sortedEigenvectors.block(0, 0, halfRows, sortedEigenvectors.cols());
			Y = sortedEigenvectors.block(halfRows, 0, halfRows, sortedEigenvectors.cols());
			
			if (X.rows() == Y.cols())
			{
				Eigen::MatrixXd P = X * Y.inverse();
				Eigen::MatrixXd K = -R.inverse() * B.transpose() * P;
				return K;
			}
			else
			{
				Eigen::MatrixXd P;
				P.resize(6, 6);
				P.setConstant(NAN);
				return P;
			}
			
			/*std::cout << "X dimensions: " << X.rows() << "x" << X.cols() << std::endl;
			std::cout << "Y dimensions: " << Y.rows() << "x" << Y.cols() << std::endl << "\n";
			std::cout << eigenvectorsMatrix <<"\n" << "\n";


			std::cout << "X " << X << "\n" << "\n";
			std::cout << "Y " << Y << "\n" << "\n";
			std::cout << eigenvalues << "\n" << "\n";
			std::cout << "Y-1 " << Y.inverse() << "\n" << "\n";
			std::cout << P << "\n" << "\n";*/
			
		}

		
		static void Step(Transforms & transform)
		{

			transform.acceleration = transform.force / transform.mass * dt;
			transform.velocity += transform.acceleration * dt;
			transform.position += transform.velocity * dt;
			transform.force = glm::vec3(0, 0, 0);
		}
		static void Calc_Omega(std::vector<mesh*>& All_Mesh)
		{
			glm::vec3 r = All_Mesh[0]->transform.position - All_Mesh[1]->transform.position;
			glm::vec3 v = All_Mesh[0]->transform.velocity - All_Mesh[1]->transform.velocity;

			All_Mesh[1]->transform.velocity = glm::vec3(0, 0, 0);

			omega = sqrt(G_const * (All_Mesh[0]->transform.mass + All_Mesh[1]->transform.mass) / powf(glm::length(r), 3));
			c_mass = All_Mesh[0]->transform.mass;
		}
		static void Forces(std::vector<mesh*> &All_Mesh,std::vector<float> & t_data, std::vector<float>& x_data, std::vector<float>& y_data, std::vector<float>& z_data)
		{
			glm::vec3 targetPos = glm::vec3(0, 0, 0);
			glm::vec3 targetVelocity = glm::vec3(0, 0, 0);
			float targetmass = 10.0f;
			/*for (int i = 0; i < All_Mesh.size(); i++)
			{
				glm::vec3 r = All_Mesh[i]->transform.position - targetPos;
				glm::vec3 v = All_Mesh[i]->transform.velocity - targetVelocity;

				double omega = sqrt(G_const * (All_Mesh[i]->transform.mass + targetmass) / powf(glm::length(r), 3));
				All_Mesh[i]->transform.force.x = 3 * omega * omega * r.x + 2 * omega * v.y;
				All_Mesh[i]->transform.force.y = -2 * omega * v.x;
				All_Mesh[i]->transform.force.z = -omega * omega * r.z;
				All_Mesh[i]->transform.drift = All_Mesh[i]->transform.velocity.y + 2.0f * All_Mesh[i]->transform.position.x;
				Eigen::MatrixXd K;
				PhysicsHandler::LQR_Solver(K, omega, All_Mesh[i]->transform.mass, 20);
				Eigen::MatrixXd U;
				Eigen::MatrixXd X;
				X.resize(6, 1);
				X << All_Mesh[i]->transform.position.x,
					All_Mesh[i]->transform.position.y,
					All_Mesh[i]->transform.position.z,
					All_Mesh[i]->transform.velocity.x,
					All_Mesh[i]->transform.velocity.y,
					All_Mesh[i]->transform.velocity.z;

				Eigen::MatrixXd Xd;
				Xd.resize(6, 1);
				Xd << 0,
					  0, 
					  0,
					  0,
					  0,
					  0;
				Eigen::MatrixXd A;
				A.resize(6, 6);
				A << 0, 0, 0, 1, 0, 0,
					 0, 0, 0, 0, 1, 0,
					 0, 0, 0, 0, 0, 1,
					 3 * omega * omega, 0, 0, 0, 2 * omega, 0,
					 0, 0, 0, -2 * omega, 0, 0,
				     0, 0, -omega * omega, 0, 0, 0;

				Eigen::MatrixXd B;
				B.resize(6, 6);
				B << 1 / All_Mesh[i]->transform.mass, 0, 0, 0, 0, 0,
					 0, 1 / All_Mesh[i]->transform.mass, 0, 0, 0, 0,
					 0, 0, 1 / All_Mesh[i]->transform.mass, 0, 0, 0,
				     0, 0, 0, 1 / All_Mesh[i]->transform.mass, 0, 0,
					 0, 0, 0, 0, 1 / All_Mesh[i]->transform.mass, 0,
					 0, 0, 0, 0, 0, 1 / All_Mesh[i]->transform.mass;

				U = -K*(X - Xd);
				
				All_Mesh[i]->transform.Xdot = All_Mesh[i]->transform.Xdot + (dt * A * X + B * U);
				wt = wt + dt;
				x_data.push_back(wt);
				y_data.push_back(All_Mesh[i]->transform.Xdot(0, 0));
				std::cout << "X : " << All_Mesh[i]->transform.Xdot << "Time : " << wt << "\n";
				
			}*/
			
			glm::vec3 r = All_Mesh[0]->transform.position - All_Mesh[1]->transform.position;
			glm::vec3 v = All_Mesh[0]->transform.velocity - All_Mesh[1]->transform.velocity;
			All_Mesh[0]->transform.force.x = 3 * omega * omega * r.x + 2 * omega * v.y;
			All_Mesh[0]->transform.force.y = -2 * omega * v.x;
			All_Mesh[0]->transform.force.z = -omega * omega * r.z;
			All_Mesh[0]->transform.drift = All_Mesh[0]->transform.velocity.y + 2.0f * All_Mesh[0]->transform.position.x;
			Eigen::MatrixXd K;
			//PhysicsHandler::LQR_Solver(K, omega, All_Mesh[0]->transform.mass, 10e-2);
			K = Gain;
			Eigen::MatrixXd U;
			Eigen::MatrixXd X;
			X.resize(6, 1);
			X << All_Mesh[0]->transform.position.x,
				 All_Mesh[0]->transform.position.y,
				 All_Mesh[0]->transform.position.z,
				 All_Mesh[0]->transform.velocity.x,
				 All_Mesh[0]->transform.velocity.y,
				 All_Mesh[0]->transform.velocity.z;

			Eigen::MatrixXd Xd;
			Xd.resize(6, 1);
			Xd << 0,
				0,
				0,
				0,
				0,
				0;
			Eigen::MatrixXd A;
			A.resize(6, 6);
			A << 0, 0, 0, 1, 0, 0,
				 0, 0, 0, 0, 1, 0,
				 0, 0, 0, 0, 0, 1,
				 3 * omega * omega, 0, 0, 0, 2 * omega, 0,
				 0, 0, 0, -2 * omega, 0, 0,
				 0, 0, -omega * omega, 0, 0, 0;

			Eigen::MatrixXd B;
			B.resize(6, 3);
			B << 0, 0, 0,
				 0, 0, 0,
			   	0, 0, 0,
			   	1 / All_Mesh[0]->transform.mass, 0, 0,
				0, 1 / All_Mesh[0]->transform.mass, 0,
				0, 0, 1 / All_Mesh[0]->transform.mass;


			U = -K * (X - Xd);
			//std::cout<< "This is U "<< "\n" << U << "\n";
			Eigen::MatrixXd X_next = X + dt * (A * X + B * U);
			All_Mesh[0]->transform.Xdot = X + dt * (A * X + B * U);
			wt = wt + dt;
			t_data.push_back(wt);
			x_data.push_back(X_next(0,0));
			y_data.push_back(X_next(1, 0));
			z_data.push_back(X_next(2, 0));
			All_Mesh[0]->transform.position.x = X_next(0, 0);
			All_Mesh[0]->transform.position.y = X_next(1, 0);
			All_Mesh[0]->transform.position.z = X_next(2, 0);

			All_Mesh[0]->transform.velocity.x = X_next(3, 0);
			All_Mesh[0]->transform.velocity.y = X_next(4, 0);
			All_Mesh[0]->transform.velocity.z = X_next(5, 0);

			

			//std::cout << "X : " << U << "Time : " << wt << "\n";

		}
		

};
#endif
