#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <algorithm>
#include <set>
#include "tree_msg.h"

void setAllMsgToFalse( Node & root ) {
	root.setMsg( false );
	for ( Node * child : root.getChildren() )
		setAllMsgToFalse( *child );
}

void requireAllMsgTrue( Node & root ) {
	CAPTURE( root.getID() );
	REQUIRE( root.hasMsg() );
	for ( Node * child : root.getChildren() )
		requireAllMsgTrue( *child );
}

void testUniqueAndAdd( Node * src, std::set< Node * > & seen ) {
	INFO( "Testing whether source node has not been used in this round." );
	REQUIRE( seen.find( src ) == seen.end() );
	seen.insert( src );
}

void testSchedule( Node & root, const Schedule & schedule ) {
	setAllMsgToFalse( root );
	root.setMsg( true );

	int iterCount = 0;

	for ( auto & iteration : schedule ) {
		std::set< Node * > seen;
		INFO( "Testing whether all commands in schedule are correct." );
		CAPTURE( iterCount );
		for ( auto & command : iteration ) {
			Node * src = command.first;
			Node * dst = command.second;
			CAPTURE( src->getID() );
			CAPTURE( dst->getID() );
			REQUIRE( src->hasMsg() );
			REQUIRE_FALSE( dst->hasMsg() );

			testUniqueAndAdd( src, seen );

			INFO( "Is dst really a child of src?" );
			auto & children = src->getChildren();
			auto resultOfFind = std::find(
				children.begin(),
				children.end(),
				dst );
			REQUIRE( resultOfFind != children.end() );
			dst->setMsg( true );
		}
		++iterCount;
	}

	INFO( "Testing whether all nodes have received the message." );
	requireAllMsgTrue( root );
}

TEST_CASE( "Basic test" ) {
	std::vector< Node > n;

	for ( int i = 0; i < 10; ++i )
		n.emplace_back( i );

	n[0].addChild( n[1] );
	n[0].addChild( n[2] );
	n[0].addChild( n[3] );

	n[1].addChild( n[4] );
	n[1].addChild( n[5] );
	n[1].addChild( n[6] );

	n[2].addChild( n[7] );

	n[7].addChild( n[8] );

	n[8].addChild( n[9] );

	SECTION( "hand written schedule" ) {
		Schedule sched;

		sched.push_back( { { &n[0], &n[1] } } );

		sched.push_back( { { &n[0], &n[2] },
		                   { &n[1], &n[4] } } );

		sched.push_back( { { &n[0], &n[3] },
		                   { &n[1], &n[5] },
		                   { &n[2], &n[7] } } );

		sched.push_back( { { &n[1], &n[6] },
		                   { &n[7], &n[8] } } );

		sched.push_back( { { &n[8], &n[9] } } );

		testSchedule( n[0], sched );
	}

	SECTION( "computed schedule" ) {
		Schedule sched = createSchedule( n[0] );
		testSchedule( n[0], sched );
		REQUIRE( sched.size() == 5 );
	}
}

TEST_CASE( "Custom test" ) {
    std::vector< Node > n;

    for ( int i = 0; i < 16; ++i )
        n.emplace_back( i );

    n[0].addChild( n[1] );
    n[0].addChild( n[4] );
    n[0].addChild( n[2] );
    n[0].addChild( n[3] );
    n[0].addChild( n[15] );

    n[1].addChild( n[5] );
    n[1].addChild( n[6] );
    n[1].addChild( n[7] );

    n[2].addChild( n[8]  );
    n[2].addChild( n[9]  );
    n[2].addChild( n[10] );

    n[3].addChild( n[11] );

    n[11].addChild( n[12] );

    n[12].addChild( n[13] );
    n[13].addChild( n[14] );

    // SECTION( "hand written schedule" ) {
    //     Schedule sched;

    //     sched.push_back( { { &n[0], &n[1] } } );

    //     sched.push_back( { { &n[0], &n[2] },
    //                        { &n[1], &n[4] } } );

    //     sched.push_back( { { &n[0], &n[3] },
    //                        { &n[1], &n[5] },
    //                        { &n[2], &n[7] } } );

    //     sched.push_back( { { &n[1], &n[6] },
    //                        { &n[7], &n[8] } } );

    //     sched.push_back( { { &n[8], &n[9] } } );

    //     testSchedule( n[0], sched );
    // }

    SECTION( "computed schedule" ) {
        Schedule sched = createSchedule( n[0] );
        testSchedule( n[0], sched );
        REQUIRE( sched.size() == 6 );
    }
}
