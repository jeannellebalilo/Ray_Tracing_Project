/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"

// The scene init definition 
#include "RTScene.inl"


using namespace glm;
void RTScene::buildTriangleSoup(void){
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader -> nlights = light.size();
    shader -> lightpositions.resize( shader -> nlights );
    shader -> lightcolors.resize( shader -> nlights );
    int count = 0;
    for (std::pair<std::string, Light*> entry : light){
        shader -> lightpositions[ count ] = (entry.second) -> position;
        shader -> lightcolors[ count ] = (entry.second) -> color;
        count++;
    }
    
    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack; // HW3: You will update this matrix_stack during the depth-first search while loop.
    // Initialize the current state variable for DFS
    Node* cur = node["world"]; // root of the tree
    mat4 cur_VM = glm::mat4();
    cur_VM[0] = vec4(1, 0 ,0 ,0);
    cur_VM[1] = vec4(0, 1 ,0 ,0);
    cur_VM[2] = vec4(0, 0 ,1 ,0);
    cur_VM[3] = vec4(0, 0 ,0 ,1);
     // HW3: You will update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.
    
    // HW3: The following is the beginning of the depth-first search algorithm.
    // HW3: The depth-first search for the node traversal has already been implemented (cur, dfs_stack).
    // HW3: All you have to do is to also update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);
    
    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0; 
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    
    // If you want to print some statistics of your scene graph
    // std::cout << "total numb of nodes = " << node.size() << std::endl;
    // std::cout << "total number of edges = " << total_number_of_edges << std::endl;
    
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        
        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_VM = matrix_stack.top();   matrix_stack.pop();
        
        // draw all the models at the current node
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            // Prepare to draw the geometry. Assign the modelview and the material.
            Model* model = cur->models[i];
            mat4 mTransform = cur->modeltransforms[i];

            shader -> modelview = cur_VM * mTransform; 
            mat4 trs =  cur_VM * mTransform;
            shader -> material  = ( cur -> models[i] ) -> material;
            
            shader -> setUniforms();
            
            // TODO: RAY TRACING: instead of calling "draw", dump all triangles into a list
            // (with position/normal transformed to the world coordinate)
            std::vector<Triangle> triangles = ( cur -> models[i] ) -> geometry -> elements;
            std::cout << triangles.size() << " size of triangles\n";

            mat3 A;
            A[0] = vec3(trs[0][0], trs[0][1], trs[0][2]);
            A[1] = vec3(trs[1][0], trs[1][1], trs[1][2]);
            A[2] = vec3(trs[2][0], trs[2][1], trs[2][2]);

            for (int i = 0; i<triangles.size(); i++) {
                // problem is between here
                glm::vec3 p00 = triangles[i].P[0];
                glm::vec3 p11 = triangles[i].P[1];
                glm::vec3 p22 = triangles[i].P[2];
                // and here
                glm::vec4 p0 = glm::vec4(p00, 1);
                glm::vec4 p1 = glm::vec4(p11, 1);
                glm::vec4 p2 = glm::vec4(p22, 1);
                triangles[i].P[0] = glm::vec3((trs * p0))/((trs * p0).w);
                triangles[i].P[1] = glm::vec3((trs * p1))/((trs * p1).w);
                triangles[i].P[2] = glm::vec3((trs * p2))/((trs * p2).w);
                triangles[i].N[0] = glm::normalize(glm::inverse(glm::transpose(A)) * triangles[i].N[0]);
                triangles[i].N[1] = glm::normalize(glm::inverse(glm::transpose(A)) * triangles[i].N[1]);
                triangles[i].N[2] = glm::normalize(glm::inverse(glm::transpose(A)) * triangles[i].N[2]);
                triangles[i].material = model -> material;
                

                triangle_soup.push_back(triangles[i]);
            }

            std::cout << triangle_soup.size() << " triangles in triangle_soup\n";

        }
        
        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            mat4 cTransform = cur->childtransforms[i];
            matrix_stack.push(cur_VM * cTransform);
        }
        
    } // End of DFS while loop.
}



