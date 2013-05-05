#include "info.h"
#include "pugixml.hpp"
#include <sstream>

using namespace std;
using namespace pugi;

const string Info::PARAM_NAME_X = "x";
const string Info::PARAM_NAME_Y = "y";	
const string Info::PARAM_NAME_CODE = "code";
const string Info::PARAM_NAME_CLIENT_ID = "client_id";
const string Info::PARAM_NAME_MATRIX = "matrix";
const string Info::PARAM_NAME_MATRIX_SIZE = "matrix_size";

Info::Info(void)
{
}

Info::~Info(void)
{
}

void Info::setParam(string name, string value)
{
	_params.insert(pair<string, string>(name, value));
}

string Info::toXML() 
{	
	string str = "<info><params>";
	for (map<string, string>::iterator it=_params.begin(); it!=_params.end(); ++it) {
		str += "<param><name>" + it->first + "</name><value>";
		stringstream ss;
		ss << it->second;
		str += ss.str() + "</value></param>";
	}

	str += "</params></info>";	
	return str;
}

int Info::parseXML(string str, Info &command_info)
{
	xml_document doc;
	xml_parse_result result = doc.load_buffer(str.c_str(), str.length());

	if (!result) {
		return 1;
	}

	xml_node infoDOM = doc.child("info");	

	for (xml_node paramDOM = infoDOM.child("params").child("param"); paramDOM != NULL; paramDOM = paramDOM.next_sibling()) {
		command_info.setParam(paramDOM.child_value("name"), paramDOM.child("value").text().as_string());
	}

	return 0;
}

string Info::getParam(string param_name)
{
	return _params[param_name];
}