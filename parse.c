#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cjson/cJSON.h>
#include "parse.h"

void parse_xml(const char *xml_data) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadMemory(xml_data, strlen(xml_data), "noname.xml", NULL, 0);
    if (doc == NULL) {
        printf("Failed to parse XML\n");
        return;
    }

    root_element = xmlDocGetRootElement(doc);

    printf("Root element: %s\n", root_element->name);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void parse_json(const char *json_data) {
    cJSON *json = cJSON_Parse(json_data);

    if (json == NULL) {
        printf("Failed to parse JSON\n");
        return;
    }

    cJSON *name = cJSON_GetObjectItem(json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        printf("Name: %s\n", name->valuestring);
    }

    cJSON_Delete(json);
}
