//
// Copyright (c) 2015-2017, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// Copyright (c) 2015-2017, University of Bremen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#define BOOST_TEST_MODULE GridTest
#include <boost/test/unit_test.hpp>

#include <maps/grid/MultiLevelGridMap.hpp>

#include <maps/grid/GridAccessInterface.hpp>
#include <maps/grid/VectorGridAccess.hpp>
#include <maps/grid/GridFacade.hpp>
#include <maps/grid/LevelList.hpp>

#include <iostream>

using namespace ::maps::grid;
class PatchBase
{
    public:
        PatchBase(double m, double ma) : min(m), max(ma)
        {
        }

        virtual ~PatchBase(){};

        double min;
        double max;

        double getMiddle() const
        {
            return min + (max - min) / 2.0;
        };

        bool operator<(const PatchBase &other) const
        {
            return  getMiddle() < other.getMiddle();
        };

        double getMin() const {
            return min;
        };

        double getMax() const {
            return max;
        };

        virtual void test()
        {
            std::cout << "Base class " << std::endl; 
        };
};

class Patch : public PatchBase
{
public:
    Patch() : PatchBase(0,0)
    {
    };

    virtual ~Patch(){};

    Patch(double m, double ma) : PatchBase(m, ma)
    {
    };

    virtual void test()
    {
        std::cout << "Derived Class " << std::endl;
    };

    double someValue;
};

BOOST_AUTO_TEST_CASE(test_levelAccess)
{
    Patch p;
    
    PatchBase b = p;
    
    p.test();
    
    b.test();

    LevelList<Patch> list;
    
    LevelListAccess<PatchBase> *access = new LevelListAccessImpl<Patch, PatchBase>(&list);
    
    delete access;

}

BOOST_AUTO_TEST_CASE(test_levelAccess2)
{
    DerivableLevelList<Patch, PatchBase> list;

    std::cout << "SuperClass " << std::endl;
    
    list.begin();
    
    std::cout << "BaseClass " << std::endl;
    DerivableLevelList<PatchBase> *listBase = &list;
    
    listBase->begin();
    
    
//     LevelListAccess<PatchBase> *access = new LevelListAccessImpl<Patch, PatchBase>(&list);
    
}

BOOST_AUTO_TEST_CASE(test_mapAccess)
{
    GridMap<Patch> map;

    GridAccessInterface<PatchBase> *test = new VectorGridAccess<Patch, PatchBase>(&map);

    GridMap<PatchBase, GridFacade<PatchBase> > test2(map, GridFacade<PatchBase>(test));
}

BOOST_AUTO_TEST_CASE(test_map_access3)
{

    GridMap<Patch> map(Vector2ui(2,2), Eigen::Vector2d(1,1), Patch(0,0));
    map.at(0,0) = Patch(0, 0);
    map.at(0,1) = Patch(0, 1);
    map.at(1,0) = Patch(1, 0);
    map.at(1,1) = Patch(1, 1);

    map.at(0,0).test();
    map.at(0,1).test();

    for (auto it = map.begin(); it != map.end(); ++it)
    {
        std::cout << it->getMin() << " " << it->getMax() << std::endl;
    }

    GridAccessInterface<PatchBase> *test = new VectorGridAccess<Patch, PatchBase>(&map);

    GridMap<PatchBase, GridFacade<PatchBase> > test2(map, GridFacade<PatchBase>(test));

    test2.at(0,0).test();
    test2.at(0,1).test();

    for (auto it = test2.begin(); it != test2.end(); ++it)
    {
        std::cout << it->getMin() << " " << it->getMax() << std::endl;
    }
}

#include <ctime>

BOOST_AUTO_TEST_CASE(test_map_access_time)
{
    GridMap<Patch> map(Vector2ui(1000,1000), Eigen::Vector2d(1,1), Patch(0,0));

    clock_t begin = clock();
    for (unsigned int x = 0; x < 1000; ++x)
    {
        for (unsigned int y = 0; y < 1000; ++y)
        {
            map.at(x, y).getMin();
        }        
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    std::cout << "map: " << elapsed_secs << std::endl;

    begin = clock();
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        it->getMin();
    }
    end = clock();

    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    std::cout << "map: " << elapsed_secs << std::endl;       

    GridAccessInterface<PatchBase> *test = new VectorGridAccess<Patch, PatchBase>(&map);
    GridMap<PatchBase, GridFacade<PatchBase> > test2(map, GridFacade<PatchBase>(test));    

    begin = clock();
    for (unsigned int x = 0; x < 1000; ++x)
    {
        for (unsigned int y = 0; y < 1000; ++y)
        {
            test2.at(x, y).getMin();
        }        
    }    
    end = clock();

    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    std::cout << "test2: " << elapsed_secs << std::endl;

    auto it_2 = test2.begin();

    begin = clock();
    for (auto it = test2.begin(); it != test2.end(); ++it)
    {
        it->getMin();
    }
    end = clock();

    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    std::cout << "test2: " << elapsed_secs << std::endl;            
}

BOOST_AUTO_TEST_CASE(test_mapAccess2)
{
    GridMap<DerivableLevelList<Patch, PatchBase> > map(Vector2ui(5,5), Eigen::Vector2d(0.5,0.5), DerivableLevelList<Patch, PatchBase>());

    GridAccessInterface<DerivableLevelList<PatchBase, PatchBase> > *test = new VectorGridAccess<DerivableLevelList<Patch, PatchBase>, DerivableLevelList<PatchBase, PatchBase> >(&map);

    GridMap<DerivableLevelList<PatchBase, PatchBase>, GridFacade<DerivableLevelList<PatchBase, PatchBase> > > test2(map, GridFacade<DerivableLevelList<PatchBase, PatchBase> >(test));
    
    Patch p(38, 50);
    Patch p2(55, 80);
    
    
    map.at(2,2).insert(p2);
    map.at(2,2).insert(p);
    
    BOOST_CHECK_EQUAL(map.at(2,2).size(), 2);
    
    {
    auto it = map.at(2,2).begin();
    
    BOOST_CHECK_EQUAL(it->getMin(), 38);
    it++;
    BOOST_CHECK_EQUAL(it->getMin(), 55);
    }

    
    BOOST_CHECK_EQUAL(test2.at(2,2).size(), 2);
    
    {
    auto it = test2.at(2,2).begin();
    
    BOOST_CHECK_EQUAL(it->getMin(), 38);
    it++;
    BOOST_CHECK_EQUAL(it->getMin(), 55);

    }
    delete test;

}

/*BOOST_AUTO_TEST_CASE(test_base_class)
{

    
    MLGrid<PatchBase> grid(Vector2ui(5,5), Eigen::Vector2d(0.5,0.5));

    PatchBase p(38, 50);
    PatchBase p2(55, 80);
    
    grid.at(2,2).insert(p2);
    grid.at(2,2).insert(p);
    
    BOOST_CHECK_EQUAL(grid.at(2,2).size(), 2);
    
    {
    auto it = grid.at(2,2).begin();
    
    BOOST_CHECK_EQUAL(it->getMin(), 38);
    it++;
    BOOST_CHECK_EQUAL(it->getMin(), 55);
    }
//     for(const Patch &p : grid.at(2,2))
//         std::cout << "Bar is " << p.getMin() << std::endl;

//     Eigen::Vector3d pos;
//     grid.fromGrid(Index(2,2), pos);
//     
//     std::cout << "From" << pos.transpose() << std::endl;
    
    {
    Eigen::AlignedBox3f box(Eigen::Vector3f(0.5, .5, 49),Eigen::Vector3f(2.0, 2.0, 60));
    MLGrid<PatchBase>::MLView view = grid.intersectCuboid(box);

//     std::cout << "Subview Size : " << view.getNumCells().transpose() << std::endl;
//     for(size_t x = 0; x < view.getNumCells().x(); x++)
//     {
//         for(size_t y = 0; y < view.getNumCells().y(); y++)
//         {
//             for(const Patch *p : view.at(x,y))
//                 std::cout << "X " << x << " Y " << y << " Bar is " << p->getMin() << std::endl;
//         }
//     }
//     
    BOOST_CHECK_EQUAL(view.at(1,1).size(), 2);
    auto it = view.at(1,1).begin();
    
    BOOST_CHECK_EQUAL((*it)->getMin(), 38);
    it++;
    BOOST_CHECK_EQUAL((*it)->getMin(), 55);
    }

    {
    Eigen::AlignedBox3f box(Eigen::Vector3f(0.5, .5, 37),Eigen::Vector3f(2.0, 2.0, 38));
    MLGrid<PatchBase>::MLView view = grid.intersectCuboid(box);

    BOOST_CHECK_EQUAL(view.at(1,1).size(), 1);
    auto it = view.at(1,1).begin();
    
    BOOST_CHECK_EQUAL((*it)->getMin(), 38);
    }

    {
    Eigen::AlignedBox3f box(Eigen::Vector3f(0.5, .5, 80),Eigen::Vector3f(2.0, 2.0, 105));
    MLGrid<PatchBase>::MLView view = grid.intersectCuboid(box);

    BOOST_CHECK_EQUAL(view.at(1,1).size(), 1);
    auto it = view.at(1,1).begin();
    
    BOOST_CHECK_EQUAL((*it)->getMin(), 55);
    }
}*/


