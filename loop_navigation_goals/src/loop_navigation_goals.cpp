#include<ros/ros.h>
#include <std_msgs/Bool.h>
#include<vector>
#include<move_base_msgs/MoveBaseAction.h>
#include<actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction>MoveBaseClient;

using namespace move_base_msgs;

move_base_msgs::MoveBaseGoal buildGoal(float x, float y, float delta)
{
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = x;
    goal.target_pose.pose.position.y = y;
    goal.target_pose.pose.orientation.w = cos(delta / 2);
    goal.target_pose.pose.orientation.x = 0;
    goal.target_pose.pose.orientation.y = 0;
    goal.target_pose.pose.orientation.z = sin(delta / 2);

    return goal;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "simple_navigation_goals");
    ros::NodeHandle nh;

    std::vector<move_base_msgs::MoveBaseGoal> goalPoints;
    ros::Publisher pub_goal_finish_ = nh.advertise<std_msgs::Bool>("/goal_finish", 1);

    goalPoints.push_back(buildGoal(3.1, 0.3, 0));
    goalPoints.push_back(buildGoal(4.0, 0.3, 0));
    goalPoints.push_back(buildGoal(4.7, 0.3, M_PI_2));
    goalPoints.push_back(buildGoal(4.7, 2.4, M_PI_2 * 2));
    goalPoints.push_back(buildGoal(4.0, 2.4, M_PI_2 * 2));
    goalPoints.push_back(buildGoal(4.0, 2.4, 0));
    goalPoints.push_back(buildGoal(4.0, 2.4, M_PI_2 * 2));
    goalPoints.push_back(buildGoal(3.1, 2.4, M_PI_2 * 3));
    goalPoints.push_back(buildGoal(3.1, 1.0, M_PI_2 * 3));
    goalPoints.push_back(buildGoal(3.1, 0.3, M_PI_2 * 3));

    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);

    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))) {
        ROS_INFO("Waiting for the move_base action server to come up");
    }

    std_msgs::Bool goal_finish;
    while(true) {
        for(int i = 0; i < goalPoints.size(); i++) {
            ROS_INFO("Sending goal");
            goal_finish.data = false;
            pub_goal_finish_.publish(goal_finish);
            ac.sendGoal(goalPoints[i]);
            ac.waitForResult();

            if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
                ROS_INFO("Goal reached.");
            else
                ROS_INFO("The base failed to move forward 1 meter for some reason");

            sleep(2);
            goal_finish.data = true;
            pub_goal_finish_.publish(goal_finish);
            sleep(2);
        }
    }

    return 0;
}
