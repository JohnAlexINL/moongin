/*
    IDs look something like this:
        xx aa aa aa
    where (x) is the type and (a) is the actual index;
    typeof 0 is reserved for errors only

    For tag definitions, check glua.c or relevant headers
*/

list_t *global_item_table; // a list of lists, essentially

int item_newId(int type, int index) {
    int value = type + ( index << 8);
    return value;
}

bool item_isType(int type, int id) {
    int subtype = id % 255;
    return (type == subtype);
}

void *item_byInt(int id) {
    int type = id % 255;
    int value = id >> 8;
    if ( global_item_table->entries <= type ) { return NULL; }
    list_t *subTable = global_item_table->item[type];
    if ( subTable->entries <= value ) { return NULL; }
    return subTable->item[value];
}

void *item_byStr(const char *id){
    if (strlen(id) != 8) { return NULL; }
    int value; int i; for (i=0; i<8; i++) {
        if( id[i] <= 'f' && id[i] >= 'a' ){
            value *= 256;
            value += ( id[i] - 'a' + 10 );
        }
        else if ( id[i+2] <= '9' && id[i] >= '0' ) {
            value *= 256;
            value += ( id[i] - '0' );
        }
        else { return NULL; }
    }   return item_byInt(value);
}