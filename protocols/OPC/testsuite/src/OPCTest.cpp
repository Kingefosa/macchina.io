//
// OPCTest.cpp
//
// $Id: //poco/1.4/OPC/testsuite/src/OPCTest.cpp#1 $
//
// Copyright (c) 2010-2015, Günter Obiltschnig.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "OPCTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "IoT/OPC/Types.h"
#include "IoT/OPC/Server.h"
#include "IoT/OPC/Client.h"
#include "Poco/Thread.h"
#include "Poco/UUID.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Exception.h"
#include <iostream>


using namespace Poco;
using namespace Poco::Dynamic;
using namespace IoT::OPC;


OPCTest::OPCTest(const std::string& name): CppUnit::TestCase("OPC")
{
}


OPCTest::~OPCTest()
{
}


void OPCTest::setUp()
{
}


void OPCTest::tearDown()
{
}


const UA_String& getUAString(const UA_String& uaStr)
{
	return uaStr;
}


void OPCTest::testString()
{
	String str = {};
	std::string stdStr = STDString(str);
	assert(stdStr.empty());

	const char* cstr = "abcXYZ123";
	str = UA_STRING_ALLOC(cstr);
	assert((((UA_String) STDString(str)).length) == strlen(cstr));
	assert(0 == strncmp(reinterpret_cast<const char*>(((UA_String) STDString(str)).data), cstr, str.length));
	stdStr = STDString(str);
	assert(stdStr == cstr);
	UA_free(str.data);
	str.data = 0;
	str.length = 0;

	stdStr = cstr;
	str = UA_STRING(const_cast<char*>(stdStr.c_str()));
	assert(str.length == stdStr.size());
	assert(0 == strncmp(reinterpret_cast<const char*>(str.data), stdStr.c_str(), stdStr.size()));
	str.data = 0;
	str.length = 0;

	UAString uaStr(stdStr);
	assert(str.data == 0);
	assert(str.length == 0);
	str = uaStr;
	assert(stdStr == cstr);
	assert(str.length && str.length == stdStr.size());
	assert(0 == strncmp(reinterpret_cast<const char*>(str.data), stdStr.c_str(), stdStr.size()));

	UAString uaStr2(uaStr);
	stdStr.clear();
	assert(stdStr.empty());
	stdStr = uaStr2;
	assert(stdStr == cstr);

	stdStr.clear();
	assert(stdStr.empty());
	stdStr = uaStr;
	assert(stdStr == cstr);

	stdStr = "abc123";
	UAString uaStr3(stdStr);
	assert(uaStr3.length() == stdStr.length());
	assert(std::memcmp(uaStr3.data(), stdStr.data(), stdStr.length()) == 0);
}


void OPCTest::testEmptyNodeID()
{
	NodeID nodeID;
	assert(nodeID.isNull());
	assert(!nodeID.isNumeric());
	assert(!nodeID.isString());
	assert(!nodeID.isByteString());
	assert(!nodeID.isGuid());

	try
	{
		Poco::UInt16 tmp = nodeID.getNSIndex();
		fail("calling getNSIndex() on a null nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(NullPointerException&){}

	try
	{
		NodeID::IDType tmp = nodeID.getIDType();
		fail("calling getIDType() on a null nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(NullPointerException&){}

	try
	{
		NodeID::Type tmp = nodeID.getType();
		fail("calling getType() on a null nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(NullPointerException&){}

	try
	{
		UInt32 tmp = nodeID.getNumeric();
		fail("calling getNumeric() on a null nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(NullPointerException&){}

	try
	{
		std::string str = nodeID.getString();
		fail("calling getString() on a null nodeID must throw.");
		str.clear(); // silence the compiler
	}
	catch(NullPointerException&){}

	try
	{
		NodeID::ByteStringType bstr = nodeID.getByteString();
		fail("calling getByteString() on a null nodeID must throw.");
		bstr.clear(); // silence the compiler
	}
	catch(NullPointerException&){}

	try
	{
		UUID uuid = nodeID.getGuid();
		fail("calling getGuid() on a null nodeID must throw.");
		assert(uuid.isNull()); // silence the compiler
	}
	catch(NullPointerException&){}
}


void printDataType(const UA_NodeId& nIDType)
{
	const UA_DataType* pDT = UA_findDataType(&nIDType);
	if(pDT)
	{
		std::cout << "name: " << pDT->typeName << std::endl;
		//std::cout << "ID: " << pDT->typeId << std::endl;
		std::cout << "memSize: " << pDT->memSize << std::endl;
		std::cout << "typeIndex: " << pDT->typeIndex << std::endl;
		std::cout << "membersSize: " << (int)pDT->membersSize << std::endl;
		std::cout << "builtin: " << pDT->builtin << std::endl;
		std::cout << "fixedSize: " << pDT->fixedSize << std::endl;
		std::cout << "overlayable: " << pDT->overlayable << std::endl;
		std::cout << "binaryEncodingId: " << pDT->binaryEncodingId << std::endl;
		std::cout << "members: " << pDT->members << std::endl;
	}
	else
	{
		std::cout << "Type NOT found" << std::endl;
	}
}


void OPCTest::testNumericNodeID()
{
	NodeID nodeID(1, 2);
	//printDataType(nodeID.getType());
	assert(nodeID.isNumeric());
	assert(!nodeID.isString());
	assert(!nodeID.isByteString());
	assert(!nodeID.isGuid());
	assert(!nodeID.isNull());
	assert(1 == nodeID.getNSIndex());
	assert(0 == nodeID.getIDType());
	NodeID::Type t = nodeID.getType();
	assert(1 == t.namespaceIndex);
	assert(UA_NODEIDTYPE_NUMERIC == t.identifierType);
	assert(2 == t.identifier.numeric);

	assert(2 == nodeID.getNumeric());

	try
	{
		std::string str = nodeID.getString();
		fail("calling getString() on a numeric nodeID must throw.");
		str.clear(); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		NodeID::ByteStringType bstr = nodeID.getByteString();
		fail("calling getByteString() on a numeric nodeID must throw.");
		bstr.clear(); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		UUID uuid = nodeID.getGuid();
		fail("calling getGuid() on a numeric nodeID must throw.");
		assert(uuid.isNull()); // silence the compiler
	}
	catch(InvalidAccessException&){}
}


void OPCTest::testStringNodeID()
{
	const char* cstr = "abc.123.xyz";
	NodeID nodeID(1, cstr);
	UA_DataType dt;
	UA_new(&dt);
	assert(!nodeID.isNumeric());
	assert(nodeID.isString());
	assert(!nodeID.isByteString());
	assert(!nodeID.isGuid());
	assert(!nodeID.isNull());
	assert(1 == nodeID.getNSIndex());
	assert(UA_NODEIDTYPE_STRING == nodeID.getIDType());
	NodeID::Type t = nodeID.getType();
	assert(1 == t.namespaceIndex);
	assert(UA_NODEIDTYPE_STRING == t.identifierType);
	assert(t.identifier.string.length == strlen(cstr));
	assert(0 == strncmp(cstr, (const char*) t.identifier.string.data, strlen(cstr)));

	assert(nodeID.getString() == cstr);

	try
	{
		UInt32 tmp = nodeID.getNumeric();
		fail("calling getNumeric() on a string nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		NodeID::ByteStringType bstr = nodeID.getByteString();
		fail("calling getByteString() on a string nodeID must throw.");
		bstr.clear(); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		UUID uuid = nodeID.getGuid();
		fail("calling getGuid() on a string nodeID must throw.");
		assert(uuid.isNull()); // silence the compiler
	}
	catch(InvalidAccessException&){}
}


void OPCTest::testByteStringNodeID()
{
	const char* cstr = "abc.123.xyz";
	NodeID::ByteStringType bstr(strlen(cstr));
	std::memcpy(&bstr[0], (const unsigned char*) cstr, strlen(cstr));
	NodeID nodeID(1, bstr);
	assert(!nodeID.isNumeric());
	assert(!nodeID.isString());
	assert(nodeID.isByteString());
	assert(!nodeID.isGuid());
	assert(!nodeID.isNull());
	assert(1 == nodeID.getNSIndex());
	assert(UA_NODEIDTYPE_BYTESTRING == nodeID.getIDType());
	NodeID::Type t = nodeID.getType();
	assert(1 == t.namespaceIndex);
	assert(UA_NODEIDTYPE_BYTESTRING == t.identifierType);
	assert(t.identifier.byteString.length == bstr.size());
	assert(0 == memcmp(&bstr[0], t.identifier.byteString.data, bstr.size()));

	assert(0 == memcmp(&bstr[0], &nodeID.getByteString()[0], bstr.size()));

	try
	{
		UInt32 tmp = nodeID.getNumeric();
		fail("calling getNumeric() on a string nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		std::string str = nodeID.getString();
		fail("calling getString() on a byteString nodeID must throw.");
		str.clear(); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		UUID uuid = nodeID.getGuid();
		fail("calling getGuid() on a string nodeID must throw.");
		assert(uuid.isNull()); // silence the compiler
	}
	catch(InvalidAccessException&){}
}


void OPCTest::testGuidNodeID()
{
	UUID uuid("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
	std::string uuidStr = uuid.toString();

	NodeID nodeID(1, uuid);
	assert(!nodeID.isNumeric());
	assert(!nodeID.isString());
	assert(!nodeID.isByteString());
	assert(nodeID.isGuid());
	assert(!nodeID.isNull());
	assert(1 == nodeID.getNSIndex());
	assert(UA_NODEIDTYPE_GUID == nodeID.getIDType());
	NodeID::Type t = nodeID.getType();
	assert(1 == t.namespaceIndex);
	assert(UA_NODEIDTYPE_GUID == t.identifierType);

	assert(nodeID.getGuid() == uuid);

	try
	{
		UInt32 tmp = nodeID.getNumeric();
		fail("calling getNumeric() on a Guid nodeID must throw.");
		std::memset(&tmp, 0, sizeof(tmp)); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		std::string str = nodeID.getString();
		fail("calling getString() on a Guid nodeID must throw.");
		str.clear(); // silence the compiler
	}
	catch(InvalidAccessException&){}

	try
	{
		NodeID::ByteStringType bstr = nodeID.getByteString();
		fail("calling getByteString() on a Guid nodeID must throw.");
		bstr.clear(); // silence the compiler
	}
	catch(InvalidAccessException&){}
}


void OPCTest::testDataTypeMember()
{
	DataTypeMember dtm;
	assert(0 == dtm.getIndex());
	assert(dtm.getName().empty());
	assert(0 == dtm.getPadding());
	assert(dtm.getNSZero());
	assert(!dtm.getIsArray());

	dtm.setIndex(1);
	dtm.setName("abc");
	dtm.setPadding(4);
	dtm.setNSZero(false);
	dtm.setIsArray(true);
	assert(1 == dtm.getIndex());
	assert(dtm.getName() == "abc");
	assert(4 == dtm.getPadding());
	assert(!dtm.getNSZero());
	assert(dtm.getIsArray());

	DataTypeMember::Type t;
	t.memberName = "xyz";
	t.memberTypeIndex = 2;
	t.padding = 8;
	t.namespaceZero = true;
	t.isArray = true;
	dtm.set(t);
	assert(2 == dtm.getIndex());
	assert(dtm.getName() == "xyz");
	assert(8 == dtm.getPadding());
	assert(dtm.getNSZero());
	assert(dtm.getIsArray());

	std::memset(&t, 0, sizeof(t));
	assert(0 == t.memberName);
	assert(t.memberTypeIndex == 0);
	assert(t.padding == 0);
	assert(t.namespaceZero == false);
	assert(t.isArray == false);
	t = dtm.get();
	assert(0 == strcmp(t.memberName, "xyz"));
	assert(t.memberTypeIndex == 2);
	assert(t.padding == 8);
	assert(t.namespaceZero == true);
	assert(t.isArray == true);

	std::memset(&t, 0, sizeof(t));
	assert(0 == t.memberName);
	assert(t.memberTypeIndex == 0);
	assert(t.padding == 0);
	assert(t.namespaceZero == false);
	assert(t.isArray == false);
	dtm.set(t);
	assert(0 == dtm.getIndex());
	assert(dtm.getName().empty());
	assert(0 == dtm.getPadding());
	assert(!dtm.getNSZero());
	assert(!dtm.getIsArray());
}


void OPCTest::testDateTime()
{
	IoT::OPC::DateTime dt;
	assert(0 == dt.nanosecond());
	Poco::DateTime pdt;
	dt = pdt;
	assert(dt.year() == pdt.year());
	assert(dt.month() == pdt.month());
	assert(dt.day() == pdt.day());
	assert(dt.minute() == pdt.minute());
	assert(dt.second() == pdt.second());
	assert(dt.millisecond() == pdt.millisecond());
	assert(dt.microsecond() == pdt.microsecond());
	assert(dt.nanosecond() == 0);

	UA_DateTimeStruct uaDT = UA_DateTime_toStruct(UA_DateTime_now());
	IoT::OPC::DateTime oDT(uaDT);
	assert(oDT.year() == uaDT.year);
	assert(oDT.month() == uaDT.month);
	assert(oDT.day() == uaDT.day);
	assert(oDT.minute() == uaDT.min);
	assert(oDT.second() == uaDT.sec);
	assert(oDT.millisecond() == uaDT.milliSec);
	assert(oDT.microsecond() == uaDT.microSec);
	assert(oDT.nanosecond() == uaDT.nanoSec);

	assert(IoT::OPC::DateTime::fromString(oDT.toString()) == oDT);

	IoT::OPC::DateTime sDT(oDT.toString());
	assert(sDT == oDT);

	std::string str = "05/26/2017 23:06:54.812.795.123";
	sDT = str;
	assert(sDT.toString() == str);
	assert(sDT.nanosecond() == 123);

	Var vDT = oDT;
	assert(vDT.convert<IoT::OPC::DateTime>() == oDT);
	assert(vDT.toString() == oDT.toString());
}


void OPCTest::testClient()
{
	try
	{
		Server server;

		int nsIndex = 1;
		Var id = "the.int.answer";
		std::string name = "the int answer";
		server.addVariableNode(nsIndex, id, 42, UA_TYPES_INT32, false, name, name, name);
		nsIndex = server.addNamespace("ns2");
		id = "the.double.answer";
		name = "the double answer";
		server.addVariableNode(nsIndex, id, 4.2, UA_TYPES_DOUBLE, false, name, name, name);

		id = 3;
		name = "the double answer by ID in ns2";
		server.addVariableNode(nsIndex, id, 2.4, UA_TYPES_DOUBLE, false, name, name, name);

		id = 4;
		name = "the string answer by ID in ns2";
		server.addVariableNode(nsIndex, id, std::string("abc123"), UA_TYPES_STRING, false, name, name, name);

		id = 5;
		name = "the DateTime answer by ID in ns2";
		Poco::Int64 ts = IoT::OPC::DateTime::now();
		server.addVariableNode(nsIndex, id, ts, UA_TYPES_DATETIME, false, name, name, name);

		id = 6;
		name = "the array of strings answer by ID in ns2";
		std::vector<std::string> strArr = { "abc", "123", "xyz" };
		server.addVariableNode(nsIndex, id, strArr, UA_TYPES_STRING, true, name, name, name);

		id = 7;
		name = "the array of doubles answer by ID in ns2";
		std::vector<double> dblVec = { 1.2, 2.3, 3.4 };
		server.addVariableNode(nsIndex, id, dblVec, UA_TYPES_DOUBLE, true, name, name, name);

		Thread thread;
		thread.start(server);

		Thread::sleep(100);

		Client client("localhost");
		while(!client.isConnected()) Thread::sleep(10);

		const TypeCache& cache = client.getTypeCache();
		assert(cache.has(StringNodeID(1, "the.int.answer"), UA_TYPES_INT32));
		assert(!cache.has(StringNodeID(1, "the.int.answer"), UA_TYPES_DOUBLE));
		assert(cache.has(StringNodeID(nsIndex, "the.double.answer"), UA_TYPES_DOUBLE));
		assert(!cache.has(StringNodeID(nsIndex, "the.double.answer"), UA_TYPES_INT32));
		assert(cache.has(IntNodeID(nsIndex, 3), UA_TYPES_DOUBLE));
		assert(cache.has(IntNodeID(nsIndex, 4), UA_TYPES_STRING));
		assert(!cache.has(IntNodeID(nsIndex, 4), UA_TYPES_DATETIME));
		assert(cache.has(IntNodeID(nsIndex, 5), UA_TYPES_DATETIME));
		assert(!cache.has(IntNodeID(nsIndex, 5), UA_TYPES_STRING));

		assert(!cache.has(IntNodeID(nsIndex, 55), UA_TYPES_STRING));
		assert(!cache.has(StringNodeID(33, "5"), UA_TYPES_STRING));

		nsIndex = 1;
		name = "the.int.answer";
		assert(42 == client.readInt32ByName(nsIndex, name));
		Var varVal = client.read(nsIndex, name);
		assert(varVal == 42);
		client.write(nsIndex, name, 24);
		assert(24 == client.readInt32ByName(nsIndex, name));
		varVal = client.read(nsIndex, name);
		assert(varVal == 24);

		nsIndex = 2;
		name = "the.double.answer";
		double dbl = client.readDoubleByName(nsIndex, name);
		assert(4.19 < dbl && dbl < 4.21);
		varVal = client.read(nsIndex, name);
		assert(4.19 < varVal && varVal < 4.21);
		client.write(nsIndex, name, 2.4);
		dbl = client.readDoubleByName(nsIndex, name);
		assert(2.39 < dbl && dbl < 2.41);
		varVal = client.read(nsIndex, name);
		assert(2.39 < varVal && varVal < 2.41);

		int nID = 3;
		dbl = client.readDoubleByID(nsIndex, nID);
		assert(2.3 < dbl && dbl < 2.5);
		client.write(nsIndex, nID, 4.2);
		dbl = client.readDoubleByID(nsIndex, nID);
		assert(4.19 < dbl && dbl < 4.21);

		nID = 4;
		assert(client.readStringByID(nsIndex, nID) == "abc123");
		varVal = client.read(nsIndex, nID);
		assert(varVal == "abc123");
		client.write(nsIndex, nID, "321cba");
		assert(client.readStringByID(nsIndex, nID) == "321cba");
		varVal = client.read(nsIndex, nID);
		assert(varVal == "321cba");

		nID = 5;
		assert(client.readTimestampByID(nsIndex, nID) == ts);
		varVal = client.read(nsIndex, nID);
		assert(varVal == IoT::OPC::DateTime(ts));
		try
		{
			client.write(nsIndex, nID, 1.2, true);
			fail("invalid DateTime type must fail");
		}
		catch(Poco::InvalidArgumentException&) { }

		ts = IoT::OPC::DateTime::now();
		client.write(nsIndex, nID, ts, true);
		assert(client.readTimestampByID(nsIndex, nID) == ts);
		varVal = client.read(nsIndex, nID);
		assert(varVal == IoT::OPC::DateTime(ts));

		client.writeCurrentDateTimeByID(nsIndex, nID);
		assert(client.readTimestampByID(nsIndex, nID) > ts);

		nID = 7;
		varVal = client.read(nsIndex, nID);
		std::vector<double> exDblVec = varVal.extract<std::vector<double>>();
		assert(exDblVec.size() == 3);
		assert(exDblVec == dblVec);

		nsIndex = 1;
		name = "the.int.answer";

		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		try
		{
			client.writeArray(nsIndex, name, vec);
			fail("modifying server dataype in typesafe mode must fail");
		}
		catch(Poco::InvalidAccessException&) { }

		// allow server datatype change by client and try again
		client.setTypeSafe(false);
		client.writeArray(nsIndex, name, vec);
		varVal = client.read(nsIndex, name);
		//TODO: Dynamic::Var should be capable of holding vectors of supported types as array
		assert(varVal.isArray());
		assert(varVal.size() == 3);
		vec.clear(); assert(vec.empty());
		vec = varVal.extract<std::vector<int> >();
		assert(vec.size() == 3);
		assert(vec[0] == 1);
		assert(vec[1] == 2);
		assert(vec[2] == 3);

		nsIndex = 2;
		nID = 4;
		std::vector<std::string> strVec;
		strVec.push_back("abc");
		strVec.push_back("123");
		strVec.push_back("xyz");
		client.writeArray(nsIndex, nID, strVec);
		varVal = client.read(nsIndex, nID);
		assert(varVal.isArray());
		assert(varVal.size() == 3);
		strVec.clear(); assert(strVec.empty());
		strVec = varVal.extract<std::vector<std::string> >();
		assert(strVec.size() == 3);
		assert(strVec[0] == "abc");
		assert(strVec[1] == "123");
		assert(strVec[2] == "xyz");

		client.disconnect();
		while(client.isConnected()) Thread::sleep(10);

		server.stop();
		thread.join();
	}
	catch(Poco::Exception& ex)
	{
		fail(ex.displayText());
	}
}


CppUnit::Test* OPCTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("OPCTest");

	CppUnit_addTest(pSuite, OPCTest, testString);
	CppUnit_addTest(pSuite, OPCTest, testEmptyNodeID);
	CppUnit_addTest(pSuite, OPCTest, testNumericNodeID);
	CppUnit_addTest(pSuite, OPCTest, testStringNodeID);
	CppUnit_addTest(pSuite, OPCTest, testByteStringNodeID);
	CppUnit_addTest(pSuite, OPCTest, testGuidNodeID);
	CppUnit_addTest(pSuite, OPCTest, testDataTypeMember);
	CppUnit_addTest(pSuite, OPCTest, testDateTime);
	CppUnit_addTest(pSuite, OPCTest, testClient);

	return pSuite;
}