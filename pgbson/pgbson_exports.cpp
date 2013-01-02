#include "pgbson_internal.hpp"

#include <string>
#include <cstring>

extern "C" {

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

// dummy test function
PG_FUNCTION_INFO_V1(pgbson_test);
Datum pgbson_test(PG_FUNCTION_ARGS)
{
    return return_string("OK");
}

// bson output - to json
PG_FUNCTION_INFO_V1(bson_out);
Datum
bson_out(PG_FUNCTION_ARGS)
{
    bytea* arg = GETARG_BSON(0);
    mongo::BSONObj object(VARDATA_ANY(arg));

    std::string json = object.jsonString(); // strict, not-pretty
    return return_cstring(json);
}

// bson input - from json
PG_FUNCTION_INFO_V1(bson_in);
Datum
bson_in(PG_FUNCTION_ARGS)
{
    char* arg = PG_GETARG_CSTRING(0);
    try
    {
        mongo::BSONObj object = mongo::fromjson(arg, NULL);
        // copy to palloc-ed buffer
        return return_bson(object);
    }
    catch(...)
    {
        ereport(
            ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for BSON"))
        );
    }
}

PG_FUNCTION_INFO_V1(bson_get_string);
Datum
bson_get_string(PG_FUNCTION_ARGS)
{
    return bson_get<std::string>(fcinfo);
}

// Converts composite type to BSON
//
// Code of this function is based on row_to_json
PG_FUNCTION_INFO_V1(row_to_bson);
Datum
row_to_bson(PG_FUNCTION_ARGS)
{
    std::cout << "row_to_bson" << std::endl;
    Datum record = PG_GETARG_DATUM(0);
    mongo::BSONObjBuilder builder;

    composite_to_bson(builder, record);

    return return_bson(builder.obj());
}


}