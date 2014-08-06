#ifndef ARM_ANALYTICAL_INVERSE_KINEMATICS_H
#define ARM_ANALYTICAL_INVERSE_KINEMATICS_H

#include <vector>

// Package includes
#include <youbot_arm_kinematics/inverse_kinematics.h>

/**
 * An analytical IK solver for the youBot arm.
 */
class ArmAnalyticalInverseKinematics : public InverseKinematics
{
	public:
		/**
		 * Ctor.
		 *
		 * @param min_angles The minimum joint limits of the arm. The values are
		 * 						provided as radians [rad].
		 * @param max_angles The maximum joint limits of the arm. The values are
		 * 						provided as radians [rad].
		 */
		ArmAnalyticalInverseKinematics(
			const std::vector<double> &min_angles,
			const std::vector<double> &max_angles);

		/**
		 * Dtor.
		 */
		virtual ~ArmAnalyticalInverseKinematics();

		/**
		 * @see InverseKinematics::CartToJnt
		 */
		int CartToJnt(const KDL::JntArray& q_init,
			const KDL::Frame& p_in,
			std::vector<KDL::JntArray>& q_out);

		/**
		 * @see InverseKinematics::getSolverInfo
		 */
		void getSolverInfo(kinematics_msgs::KinematicSolverInfo &response);


	private:
		/**
		 * Tell if a solution is inside of the joint limits.
		 *
		 * @param solution The solution that has been generated by the IK
		 * 					solver.
		 * @return True if the solution is valid else false.
		 */
		bool isSolutionValid(const KDL::JntArray &solution) const;

		/**
		 * The IK solver. It solves for translation (x, y, z) as well as roll
		 * and pitch of the final arm pose. A pose with yaw != 0 cannot be
		 * solved, so it's not possible to provide the yaw.
		 *
		 * The "offset" parameters allow searching for redundant solutions.
		 * Joint 1 can either be pointing towards the position or away from the
		 * position. Joint 3 can either be up or down.
		 *
		 * The IK solver is based on a geometric approach:
		 * - Joint 1 determines the "pointing" direction of the arm. It can be
		 *   considered in isolation by only looking at the x-y-plane. This
		 *   enables us to treat the remaining joints in a plane that is
		 *   perpendicular to the x-y-plane.
		 * - Joint 2 and joint 3 determine the height and distance of the
		 *   gripper (seen in the frame of joint 2). They have to be considered
		 *   in combination, which also leads to a redundant solution.
		 *   The height and distance of the gripper depend on the pitch of the
		 *   final.
		 * - Joint 4 determines at which pitch angle the final pose is
		 *   approached.
		 * - Joint 5 determines at which roll angle the final pose is
		 *   approached. It can be considered in isolation. There are two
		 *   possible solutions: One is the provided roll and the other is the
		 *   provided roll plus pi.
		 *
		 *
		 * @param x: X coordinate of the cartesian position
		 * @param y: Y coordinate of the cartesian position
		 * @param z: Z coordinate of the cartesian position
		 * @param roll: Roll of the gripper
		 * @param pitch: Pitch of the gripper
		 * @param offset_joint_1: Determines the result of the first joint
		 * @param offset_joint_3: Determines the result of the third joint
		 * @param offset_joint_5: Determines the result of the fifth joint
		 * @return: If there is a solution a JntArray with five entries is
		 * 			returned, else the JntArray is empty.
		 */
		KDL::JntArray ik(const KDL::Frame& frame, bool offset_joint_1 = false,
				bool offset_joint_3 = false, bool offset_joint_5 = false);

		KDL::Frame ProjectGoalOrientationIntoArmSubspace(const KDL::Frame &goal) const;


	private:
		/**
		 * Minimum joint limits.
		 */
		std::vector<double> _min_angles;

		/**
		 * Maximum joint limits.
		 */
		std::vector<double> _max_angles;

		/**
		 * Information about the IK solver.
		 */
		kinematics_msgs::KinematicSolverInfo _solver_info;

		/**
		 * Threshold when a number is near to +1 or -1. This is required to
		 * prevent invalid arguments to acos.
		 */
		static const double ALMOST_PLUS_ONE;
		static const double ALMOST_MINUS_ONE;
};

#endif
