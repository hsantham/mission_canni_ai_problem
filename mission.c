#include <stdio.h>
#include <stdlib.h>
#include <queue.h>

// Number of humans and cannibals
#define COUNT 3

/*
   3 Humans,3 Cannibals, 1 boat ============== 0 Humans, 0 Cannibals, 0 boat
   (Side 1 of the river)                       (Side 2 of the river)
*/

struct state_
{
    // Number of humans, cannibals
    // boat is either 1 or 0
    int human, canni, boat;
};
typedef struct state_ state_t;

// Action could either
// 1) Sending x humans and y cannibals (or)
// 2) Receiving x humans and y cannibals.
struct action_
{
    // isSent - Captures whether sending or receiving action
    // Count of humans and cannibals sent/received is indicated
    // in "human,canni"
    int human, canni, isSent;
};
typedef struct action_ action_t;

// Representing a generated node in tree
// Nodes in the tree correspond to side 1 of the river.
struct tree_node_
{
    state_t            state;
    struct tree_node_* parent;
    action_t           action;
    int                path_cost;
    int                depth;
};
typedef struct tree_node_ tree_node_t;

action_t possible_actions[] = {
        {1,0,1},
        {0,1,1},
        {1,1,1},
        {2,0,1},
        {0,2,1},
        /* Remove this comment if boat capacity
           has to be increased to 3
        {2,1,1},
        {1,2,1},
        {3,0,1},
        {0,3,1},
        */
        {1,0,0},
        {0,1,0},
        {1,1,0},
        {2,0,0},
        {0,2,0},
        /* Remove this comment if boat capacity
           has to be increased to 3
        {2,1,0},
        {1,2,0},
        {3,0,0},
        {0,3,0},
        */
};

state_t goal_state = {0,0,0};

int compare_state_struct(state_t state1, 
                         state_t state2);

int goal_test(state_t current_state)
{
    return compare_state_struct(current_state,goal_state);
}

/* Generate the goal state by applying action <action>
   to parent node <parent>
 */
state_t result_state(tree_node_t *parent,
                            action_t action)
{
    state_t result;

    // Decrease count if we are sending humans, cannibals
    // from side 1 to side 2
    if(action.isSent)
    {
       result.human = parent->state.human - action.human;
       result.canni = parent->state.canni - action.canni;
       result.boat  = parent->state.boat  - 1;
    }
    else 
    {
    // Increase count if we are receiving humans, cannibals
    // from side 2 to side 1
       result.human = parent->state.human + action.human;
       result.canni = parent->state.canni + action.canni;
       result.boat  = parent->state.boat  + 1;
    }

    return result;
}

// Create a node in the tree
tree_node_t *create_node(state_t        state,
                         tree_node_t*   parent,
                         action_t       action,
                         int            path_cost,
                         int            depth)
{
    tree_node_t *node = malloc(sizeof(tree_node_t));
    assert(node);

    node->state = state;
    node->parent = parent;
    node->action = action;
    node->path_cost = path_cost;
    node->depth = depth;

    return node;
}

// Compare two states for equality.
int compare_state_struct(state_t state1, 
                         state_t state2)
{
    return state1.human == state2.human &&
           state1.canni == state2.canni &&
           state1.boat  == state2.boat;
}

tree_node_t * create_child_node(tree_node_t* parent, action_t action)
{
    return create_node(result_state(parent, action),
                       parent,
                       action,
                       parent->path_cost + 1,
                       parent->depth + 1);
}

int isApplicable(tree_node_t * node, action_t action)
{
    // Receive action is not possible if boat is in 
    // side 1 of the river
    if(node->state.boat == 1 && 
       action.isSent == 0 /* isSent == 0 means receive action */)
    {
        return 0;
    }

    // Send action is not possible if boat is in 
    // side 2 of the river
    if(node->state.boat == 0 && action.isSent == 1)
    {
        return 0;
    }

    state_t side1State = node->state;
    state_t side2State = {COUNT - node->state.human,
                          COUNT - node->state.canni,
                          1     - node->state.boat };

    // Compute state change in both sides of the river
    // if action <action> is performed.
    if(action.isSent == 1)
    {
        side1State.human = side1State.human - action.human;
        side1State.canni = side1State.canni - action.canni;
        side1State.boat  = side1State.boat  - 1;

        side2State.human = side2State.human + action.human;
        side2State.canni = side2State.canni + action.canni;
        side2State.boat  = side2State.boat  + 1;
    }
    else 
    {
        side1State.human = side1State.human + action.human;
        side1State.canni = side1State.canni + action.canni;
        side1State.boat  = side1State.boat  + 1;

        side2State.human = side2State.human - action.human;
        side2State.canni = side2State.canni - action.canni;
        side2State.boat  = side2State.boat  - 1;
    }

    // Return 1 if performing action <action> leads to valid conditions
    // like humans count is more than or equal to cannibals count on
    // both sides of the river.
    return (!side1State.human || (side1State.human >= side1State.canni)) &&
           (!side2State.human || (side2State.human >= side2State.canni)) &&
           (side1State.human >= 0 && side1State.canni >=0 && side1State.boat >= 0) &&
           (side2State.human >= 0 && side2State.canni >=0 && side2State.boat >= 0);
}

// Traverse through the queue to find whether the state <state> 
// already exists in the queue.
tree_node_t * is_exists(q_node_t *queue, state_t state)
{
    q_node_t *node = queue;
    while(node)
    {
        tree_node_t* tnode = (tree_node_t*) queue->value;
        if(compare_state_struct(tnode->state,state))
        {
            return tnode;
        }
        node = node->next;
    }

    return NULL;
}

void print_node_state(state_t state)
{
    printf("==========NODE State: %d %d %d ", 
            state.human, state.canni, state.boat);
    printf(" || %d %d %d ==========\n", 
            COUNT - state.human, 
            COUNT - state.canni, 
            1 - state.boat);
}

void print_child_node_state(state_t state)
{
    printf("CHILD NODE State: %d %d %d ", 
            state.human, state.canni, state.boat);
    printf(" || %d %d %d \n", 
            COUNT - state.human, 
            COUNT - state.canni, 
            1 - state.boat);
}

void print_action(action_t action)
{
      if(action.isSent)
      {
          printf("=> Sending %d human %d canni;\n", 
                  action.human, action.canni);
      }
      else
      {
          printf("=> Rcving  %d human %d canni;\n", 
                  action.human, action.canni);
      }
}

// BFS tree generation starting from initial state
tree_node_t * BFS(state_t initial_state)
{
    action_t ac = {};
    tree_node_t *node = create_node(initial_state, NULL, ac, 0, 0);

    if(goal_test(node->state) == 1)
    {
        return node;
    }

    q_node_t *frontier = NULL;
    q_node_t *explored = NULL;

    insert(&frontier, node);

    while(1)
    {
        if(frontier == NULL)
        {
            return NULL;
        }

        node = pop(&frontier);
        insert(&explored, node);

        print_node_state(node->state);

        int total_actions_count = (sizeof(possible_actions)/sizeof(action_t));
        for(int i = 0; i < total_actions_count; i++)
        {
            action_t action = possible_actions[i];
            if(!isApplicable(node, action))
                continue;

            tree_node_t *child_node = create_child_node(node, action);
            if(!is_exists(frontier, child_node->state) && !is_exists(explored, child_node->state))
            {
                if(goal_test(child_node->state) == 1)
                {
                    return child_node;
                }

                print_child_node_state(child_node->state);
                insert(&frontier, child_node);
            }
        }
    }
}

// Print the solution path from root node
// Recursively traverse through to the top and 
// then print the solution
void print_soln(tree_node_t *node)
{
    if(node == NULL)
        return;

    print_soln(node->parent);
    if(node->parent)
    {
      print_action(node->action);
    }

    // Print the node's state i.e., Humans and cannibals
    // count on both sides of the river.
    printf("%d %d %d ",
            node->state.human,
            node->state.canni,
            node->state.boat);
    printf(" || %d %d %d \n", 
            COUNT - node->state.human, 
            COUNT - node->state.canni, 
            1 - node->state.boat);
}

int main()
{
    state_t initial_state = {COUNT,COUNT,1};
    tree_node_t *soln = BFS(initial_state);
    printf("\n\n================PRINTING SOLUTION=============\n\n");
    print_soln(soln);
    return 0;
}
