#include "skeletonUpdater.h"

#include "external/JM/point3.h"
#include "external/JM/MEC.h"
#include "external/JM/MVCoordinates3D.h"

SkeletonUpdater::SkeletonUpdater()
{

}

SkeletonUpdater::SkeletonUpdater(Weights   *_wSkeleton,
                                 Weights   *_wCage,
                                 Character *_character,
                                 Skeleton  *_skeleton,
                                 Cage      *_cage)
{

   create(_wSkeleton,
          _wCage,
          _character,
          _skeleton,
          _cage);
}

bool SkeletonUpdater::create(Weights *_wSkeleton,
                             Weights *_wCage,
                             Character *_character,
                             Skeleton *_skeleton,
                             Cage *_cage)
{
   clear();

   setSkeletonWeights(_wSkeleton);
   setCageWeights(_wCage);
   setCharacter(_character);
   setSkeleton(_skeleton);
   setCage(_cage);

   generateSkeletonUpdaterWeights(skeletonUpdaterWeights,
                                  wSkel,
                                  wCage,
                                  character,
                                  skeleton,
                                  cage);

   originalNodePositions.resize(skeleton->getNumNodes());

   for( unsigned long i = 0 ; i < skeleton->getNumNodes() ; ++i )
   {
      originalNodePositions[i] = skeleton->getLocalNodePosition(i);
   }

   return true;
}

void SkeletonUpdater::clear()
{
   wSkel = nullptr;
   wCage = nullptr;
   character = nullptr;
   skeleton = nullptr;
   cage = nullptr;

   skeletonUpdaterWeights = nullptr;;

   originalNodePositions.clear();
}



//bool SkeletonUpdater::generateSkeletonUpdaterWeights(
//        Weights   * & skeletonUpdaterWeights,
//        Weights   * skeletonWeights,
//        Weights   * cageWeights,
//        Character * character,
//        Skeleton  * skeleton,
//        Cage      * cage)
//{

//    std::cout << "\t find_weights_for_articulations" << std::endl;
//  //  std::cout << "\t\t skeleton->numNodes() : " << skeleton->numNodes() << std::endl;
//  //  std::cout << "\t\t cage->getNumVertices() : " << cage->getNumVertices() << std::endl;
//  //  std::cout << "\t\t character->getNumVertices() : " << character->getNumVertices() << std::endl;

//    skeletonUpdaterWeights = new Weights(skeleton->numNodes(),cage->getNumVertices());

//    std::vector< jm::point3d > cage_vertices( cage->getNumVertices() );
//    const std::vector<double> & cg3Vertices = cage->getRestPoseVerticesVector();
//    ;

//    for( unsigned long v = 0 ; v < cage->getNumVertices() ; ++v )
//    {
//        jm::point3d p(cg3Vertices[v*3+0],cg3Vertices[v*3+1],cg3Vertices[v*3+2]);
//        cage_vertices[v] = p;
//    }

//    double sExponent = 0.05; // jm: fine-tuned this parameter
//    double epsilon = 0.00001;
//    for( unsigned long j = 0 ; j < skeleton->numNodes() ; ++j ) {
//        std::vector<int> bonesAdj;
//        int father = skeleton->getNode(j).getFather();
//        if(father!=-1)
//            bonesAdj.push_back(father);
//        bonesAdj.push_back(j);

//        cg3::Point3d cg3JointJ = skeleton->getNode(j).getGlobalRestPose().getTranslation();
//        jm::point3d jointPosition(cg3JointJ[0],cg3JointJ[1],cg3JointJ[2]);

//        std::vector< double > weightPrior( character->getNumVertices() , 0.0 );
//        for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v ) {
//            double localityFactor = -1;
//            for(int b:bonesAdj)
//            {
//                double w_vb = skeletonWeights->getWeight(b,v);
//                {
//                    localityFactor += pow(std::max<double>(epsilon,fabs(w_vb)) , sExponent); // jm: add max(epsilon, . ) to handle 0 weights joints ("fake" joints)
//                }
//            }
//            cg3::Vec3d vi = character->getRestPoseVertex(v);
//            localityFactor = std::max<double>(localityFactor , epsilon);

//            double dist = ((vi-cg3JointJ).length());
//            double mvcFactor = jm::point3d::dot( character->getRestPoseVertexNormal(v) , vi-cg3JointJ )  / (dist * dist * dist * dist * dist +epsilon);
//            localityFactor *= mvcFactor;   //FABRIZIO: TEST


//            weightPrior[v] = localityFactor; // HERE : MULTIPLY BY THE AREA AROUND THE VERTEX ??? (it should be, it would make the process insensitive to tesselation)
//        }

//        std::vector< double > jointWeightsInvalid( cage->getNumVertices() , 0.0 );
//        for(int c = 0; c < cage->getNumVertices(); ++c)
//        {
//            for(int v = 0; v < character->getNumVertices(); ++v)
//            {
//                jointWeightsInvalid[c] += weightPrior[v] * cageWeights->getWeight(c,v);
//            }
//        }

//        std::vector< double > jointWeights( cage->getNumVertices() , 0.0 );
//        MEC::computeCoordinates< jm::mat33d >(jointPosition , cage_vertices , jointWeightsInvalid , jointWeights);

//        for(int c = 0; c < cage->getNumVertices(); ++c)
//        {
//            skeletonUpdaterWeights->setWeight(c,j,jointWeights[c]);
//        }
//    }
//    std::cout << "\t find_weights_for_articulations done" <<std::endl;

//    return true;
//}



//bool SkeletonUpdater::generateSkeletonUpdaterWeights(
//        Weights   * & skeletonUpdaterWeights,
//        Weights   * skeletonWeights,
//        Weights   * cageWeights,
//        Character * character,
//        Skeleton  * skeleton,
//        Cage      * cage)
//{

//    std::cout << "\t find_weights_for_articulations" << std::endl;
//    std::vector< std::vector< double > > AVWeights;
//    AVWeights.resize( skeleton->numNodes() );
//    std::vector< jm::point3d > mesh_vertices( character->getNumVertices() );
//    std::vector< std::vector< int > > mesh_triangles( character->getNumTriangles() );
//    const std::vector<double> & cg3Vertices = character->getRestPoseVerticesVector();
//    const std::vector<int> & cg3Tris = character->getTrianglesVector();
//    for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v )
//    {
//        jm::point3d p(cg3Vertices[v*3+0],cg3Vertices[v*3+1],cg3Vertices[v*3+2]);
//        mesh_vertices[v] = p;
//    }

//    for( unsigned long t = 0 ; t < character->getNumTriangles() ; ++t )
//    {
//        std::vector<int> tv(3);
//        tv[0] = cg3Tris[t*3+0];
//        tv[1] = cg3Tris[t*3+1];
//        tv[2] = cg3Tris[t*3+2];
//        mesh_triangles[t] = tv;
//    }

//    double sExponent = 0.05; // jm: fine-tuned this parameter
//    double epsilon = 0.00001;
//    for( unsigned long j = 0 ; j < skeleton->numNodes() ; ++j ) {

//        AVWeights[j].resize( character->getNumVertices() );
//        std::vector<int> bonesAdj;
//        int father = skeleton->getNode(j).getFather();
//        if(father!=-1)
//            bonesAdj.push_back(father);
//        bonesAdj.push_back(j);

//        cg3::Point3d cg3JointJ = skeleton->getNode(j).getGlobalRestPose().getTranslation();
//        jm::point3d p(cg3JointJ[0],cg3JointJ[1],cg3JointJ[2]);

//        std::vector< double > weightPrior( character->getNumVertices() , 0.0 );
//        //initialize weightPrior with the MVC of p with respect to the character
//        MVCoordinates::MVC3D::computeCoordinates( p,
//                                                  mesh_triangles ,
//                                                  mesh_vertices ,
//                                                  weightPrior );
//        // then
//        for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v ) {
//            double localityFactor = -1;
//            for(int b:bonesAdj)
//            {
//                double w_vb = skeletonWeights->getWeight(b,v);
//                {
//                    localityFactor += pow(std::max<double>(epsilon,fabs(w_vb)) , sExponent); // jm: add max(epsilon, . ) to handle 0 weights joints ("fake" joints)
//                }
//            }
//            localityFactor = std::max<double>(localityFactor , epsilon);
//            weightPrior[v] *= localityFactor;
//        }
//        double sumPriors = 0.0;
//        for( unsigned int v = 0 ; v < weightPrior.size() ; ++v ) {
//            sumPriors += weightPrior[v];
//        }

//        MEC::computeCoordinates< jm::mat33d >(p , mesh_vertices , weightPrior , AVWeights[j]);
//    }
//    std::cout << "\t find_weights_for_articulations done" <<std::endl;

//    {
//        //Express the skeleton nodes as a linear combination of the cage vertices:
//        skeletonUpdaterWeights =
//                new Weights(skeleton->numNodes(),cage->getNumVertices());
//        for(int j = 0; j < skeletonUpdaterWeights->getNumberOfVertices(); ++j)
//        {
//            for(int c = 0; c < cage->getNumVertices(); ++c)
//            {
//                double phi_jc = 0.0;
//                for(int v = 0; v < character->getNumVertices(); ++v)
//                {
//                    phi_jc += AVWeights[j][v] * cageWeights->getWeight(c,v);
//                }
//                skeletonUpdaterWeights->setWeight(c,j,phi_jc);
//            }
//        }
//    }

//    return true;
//}



bool SkeletonUpdater::generateSkeletonUpdaterWeights(
      Weights   * & skeletonUpdaterWeights,
      Weights   * skeletonWeights,
      Weights   * cageWeights,
      Character * character,
      Skeleton  * skeleton,
      Cage      * cage)
{

   std::cout << "\t find_weights_for_articulations" << std::endl;
   std::vector< std::vector< double > > AVWeights;
   AVWeights.resize( skeleton->getNumNodes() );
   std::vector< jm::point3d > mesh_vertices( character->getNumVertices() );
   std::vector< std::vector< int > > mesh_triangles( character->getNumTriangles() );
   const std::vector<double> & cg3Vertices = character->getRestPoseVerticesVector();
   const std::vector<int> & cg3Tris = character->getTrianglesVector();


   #pragma omp parallel for schedule(static)
   for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v )
   {
      jm::point3d p(cg3Vertices[v*3+0],cg3Vertices[v*3+1],cg3Vertices[v*3+2]);
      mesh_vertices[v] = p;
   }


   #pragma omp parallel for schedule(static)
   for( unsigned long t = 0 ; t < character->getNumTriangles() ; ++t )
   {
      std::vector<int> tv(3);
      tv[0] = cg3Tris[t*3+0];
      tv[1] = cg3Tris[t*3+1];
      tv[2] = cg3Tris[t*3+2];
      mesh_triangles[t] = tv;
   }

   std::vector< jm::point3d > cage_vertices( cage->getNumVertices() );
   const std::vector<double> & cg3cage_vertices = cage->getRestPoseVerticesVector();


   #pragma omp parallel for schedule(static)
   for( unsigned long v = 0 ; v < cage->getNumVertices() ; ++v )
   {
      jm::point3d p(cg3cage_vertices[v*3+0],cg3cage_vertices[v*3+1],cg3cage_vertices[v*3+2]);
      cage_vertices[v] = p;
   }


   skeletonUpdaterWeights = new Weights(skeleton->getNumNodes(),cage->getNumVertices());


   double sExponent = 0.05; // jm: fine-tuned this parameter
   #pragma omp parallel for schedule(static)
   for( unsigned long j = 0 ; j < skeleton->getNumNodes() ; ++j )
   {
      double epsilon = 0.01;

      AVWeights[j].resize( character->getNumVertices() );
      std::vector<int> bonesAdj;
      int father = skeleton->getNode(j).getFather();
      if(father!=-1)
         bonesAdj.push_back(father);
      bonesAdj.push_back(j);

      cg3::Point3d cg3JointJ = skeleton->getNode(j).getGlobalTRest().getTranslation();
      jm::point3d jmJointJ(cg3JointJ[0],cg3JointJ[1],cg3JointJ[2]);

      std::vector< double > mvcoords( character->getNumVertices() , 0.0 );
      std::vector< double > weightPrior( character->getNumVertices() , 0.0 );
      //initialize weightPrior with the MVC of p with respect to the character
      MVCoordinates::MVC3D::computeCoordinates( jmJointJ ,
                                                mesh_triangles ,
                                                mesh_vertices ,
                                                mvcoords );
      double sumPriors = 0.0;

      // then
      for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v ) {
         double localityFactor = -1;
         for(int b:bonesAdj)
         {
            double w_vb = skeletonWeights->getWeight(b,v);
            {
               localityFactor += pow(std::max<double>(epsilon,fabs(w_vb)) , sExponent); // jm: add max(epsilon, . ) to handle 0 weights joints ("fake" joints)
            }
         }
         localityFactor = std::max<double>(localityFactor , epsilon);

         //if( std::isnan(localityFactor) ) std::cout << "   localityFactor is NaN , for v = " << v << std::endl;
         //if( std::isnan(weightPrior[v]) ) std::cout << "   mvc[v] is NaN , for v = " << v << std::endl;

         weightPrior[v] = mvcoords[v] * localityFactor;
         //TO DO: Try to undertstand why this can fail

         sumPriors += weightPrior[v];
      }

      //std::cout << "   sumPriors = " << sumPriors << " for j = " << j << std::endl;


      {
         //Express the skeleton nodes as a linear combination of the cage vertices:
         std::vector< double > jointWeightsInvalid( cage->getNumVertices() , 0.0 );
         for(int c = 0; c < cage->getNumVertices(); ++c)
         {
            for(int v = 0; v < character->getNumVertices(); ++v)
            {
               jointWeightsInvalid[c] += weightPrior[v] * cageWeights->getWeight(c,v);
            }
         }

         //TEST
         std::vector< double > charSkelWeights( character->getNumVertices() , 0.0 );
         MEC::computeCoordinates< jm::mat33d >( jmJointJ , mesh_vertices , weightPrior , charSkelWeights , 100 , 50 , 0.001 );

         //END TEST


         std::vector< double > jointWeights( cage->getNumVertices() , 0.0 );
         MEC::Stats mecStats = MEC::computeCoordinates< jm::mat33d >( jmJointJ , cage_vertices , jointWeightsInvalid , jointWeights , 100 , 50 , 0.001 );

         //std::cout << "   mecStats for j = " << j << "  :  isNaN = " << mecStats.isNaN << " , with linearPrecisionError = "
         //          << mecStats.linearPrecisionError << " after " << mecStats.it << " iterations" << std::endl;

         // AWFUL safe guard:
         if( mecStats.linearPrecisionError > 0.000001 ) {
            std::cout << "MEC hass failed with the LBS derived prior. Computation without it" << std::endl;
            jointWeightsInvalid.clear();
            jointWeightsInvalid.resize(cage->getNumVertices() , 0.0 );
            for(int c = 0; c < cage->getNumVertices(); ++c)
            {
               for(int v = 0; v < character->getNumVertices(); ++v)
               {
                  jointWeightsInvalid[c] += mvcoords[v] * cageWeights->getWeight(c,v);
               }
            }
            mecStats = MEC::computeCoordinates< jm::mat33d >( jmJointJ , cage_vertices , jointWeightsInvalid , jointWeights , 100 , 50 , 0.001 );

            //std::cout << "   mecStats for j = " << j << "  :  isNaN = " << mecStats.isNaN << " , with linearPrecisionError = "
            //          << mecStats.linearPrecisionError << " after " << mecStats.it << " iterations" << std::endl;
         }

         for(int c = 0; c < cage->getNumVertices(); ++c)
         {
            skeletonUpdaterWeights->setWeight(c,j,jointWeights[c]);
         }
      }
   }
   std::cout << "\t find_weights_for_articulations done" <<std::endl;

   return true;
}


void SkeletonUpdater::updatePosition()
{
   const std::vector<double> & cageVerticesRest = cage->getRestPoseVerticesVector();

   //#pragma omp parallel for schedule(static)
   for(unsigned long j=0; j<skeleton->getNumNodes(); ++j)
   {
      cg3::Point3d pRest;
      for( unsigned long c = 0 ; c < cage->getNumVertices() ; ++c )
      {
         double w = skeletonUpdaterWeights->getWeight(c,j);
         pRest[0] += w * cageVerticesRest[c*3+0];
         pRest[1] += w * cageVerticesRest[c*3+1];
         pRest[2] += w * cageVerticesRest[c*3+2];
      }

      skeleton->getNode(j).getGlobalTRest().setTranslation(pRest);
   }

   skeleton->updateLocalFromGlobalRest();

   for(unsigned long j=0; j<skeleton->getNumNodes() ; ++j)
   {
      cg3::Vec3d t = skeleton->getNode(j).getLocalTRest().getTranslation();
      skeleton->getNode(j).getLocalTCurrent().setTranslation(t);

      //cg3::Transform test = skeleton->getNode(j).getLocalTCurrent().cumulateWith(skeleton->getNode(j).getLocalTRest().inverse());
      //std::cout << "T : " << t << skeleton->getNode(j).getLocalTCurrent().getTranslation() << test.getTranslation() << std::endl;
   }

   skeleton->updateGlobalFromLocalCurrent();
}

bool SkeletonUpdater::generateSkeletonUpdaterWeights(Weights   * & skeletonUpdaterWeights,
                                                     Weights   *   skeletonWeights,
                                                     Character *   character,
                                                     Skeleton  *   skeleton)
{

   std::vector< std::vector< double > > AVWeights;
   AVWeights.resize( skeleton->getNumNodes() );
   std::vector< jm::point3d > mesh_vertices( character->getNumVertices() );
   std::vector< std::vector< int > > mesh_triangles( character->getNumTriangles() );
   const std::vector<double> & cg3Vertices = character->getRestPoseVerticesVector();
   const std::vector<int> & cg3Tris = character->getTrianglesVector();


   #pragma omp parallel for schedule(static)
   for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v )
   {
      jm::point3d p(cg3Vertices[v*3+0],cg3Vertices[v*3+1],cg3Vertices[v*3+2]);
      mesh_vertices[v] = p;
   }


   #pragma omp parallel for schedule(static)
   for( unsigned long t = 0 ; t < character->getNumTriangles() ; ++t )
   {
      std::vector<int> tv(3);
      tv[0] = cg3Tris[t*3+0];
      tv[1] = cg3Tris[t*3+1];
      tv[2] = cg3Tris[t*3+2];
      mesh_triangles[t] = tv;
   }

   skeletonUpdaterWeights = new Weights(skeleton->getNumNodes(),character->getNumVertices());


   double sExponent = 0.05; // jm: fine-tuned this parameter
   for( unsigned long j = 0 ; j < skeleton->getNumNodes() ; ++j )
   {
      double epsilon = 0.01;

      AVWeights[j].resize( character->getNumVertices() );
      std::vector<int> bonesAdj;
      int father = skeleton->getNode(j).getFather();
      if(father!=-1)
         bonesAdj.push_back(father);
      bonesAdj.push_back(j);

      cg3::Point3d cg3JointJ = skeleton->getNode(j).getGlobalTRest().getTranslation();
      jm::point3d jmJointJ(cg3JointJ[0],cg3JointJ[1],cg3JointJ[2]);

      std::vector< double > mvcoords( character->getNumVertices() , 0.0 );
      std::vector< double > weightPrior( character->getNumVertices() , 0.0 );
      //initialize weightPrior with the MVC of p with respect to the character
      MVCoordinates::MVC3D::computeCoordinates( jmJointJ ,
                                                mesh_triangles ,
                                                mesh_vertices ,
                                                mvcoords );
      double sumPriors = 0.0;

      // then
      for( unsigned long v = 0 ; v < character->getNumVertices() ; ++v ) {
         double localityFactor = -1;
         for(int b:bonesAdj)
         {
            double w_vb = skeletonWeights->getWeight(b,v);
            {
               localityFactor += pow(std::max<double>(epsilon,fabs(w_vb)) , sExponent); // jm: add max(epsilon, . ) to handle 0 weights joints ("fake" joints)
            }
         }
         localityFactor = std::max<double>(localityFactor , epsilon);
         weightPrior[v] = mvcoords[v] * localityFactor;
         sumPriors += weightPrior[v];
      }

      {
         std::vector< double > charSkelWeights( character->getNumVertices() , 0.0 );
         MEC::computeCoordinates< jm::mat33d >( jmJointJ , mesh_vertices , weightPrior , charSkelWeights , 100 , 50 , 0.001 );
         for(int v = 0; v < character->getNumVertices(); ++v)
         {
            skeletonUpdaterWeights->setWeight(v,j,charSkelWeights[v]);
         }
      }
   }
   return true;
}