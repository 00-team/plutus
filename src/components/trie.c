
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "../plutus.h"

// digits 0-9
#define NODE_ABC 10

// this two compine must be 9
// 09xx-xxx-xxxx
#define CACHE_LVL 1
// cache size is 10 ** CACHE_LVL
// 3: 10 x 10 x 10
#define CACHE_SIZ 10
#define LINKS_LEN 9 - CACHE_LVL

FILE *phdb = NULL;
FILE *pidb = NULL;

// link is a number between 0 and 9
typedef unsigned char link_t;
// Node is a sequence of positions in the file to other nodes
// 0: pos of a node
// 1: empty
// ...
// 9: pos of a node
typedef size_t Node[NODE_ABC];
// links is a sequence of numbers between 0 and 9
// e.g.: 0 9 1 2 2 2
// which is the same as the phone number
typedef link_t Links[LINKS_LEN];

// phone struct
typedef struct {
    size_t index;
    Links links;
} Phone;

size_t root[CACHE_SIZ];

/*
// roots is the first 10 node
Node roots[NODE_ABC];
size_t tdb_size;


void print_node(Node node) {
    for (link_t i = 0; i < NODE_ABC; i++) {
        // if (i != 0) printf("|");
        printf(" %d:%-10ld", i, node[i]);
    }
    printf("\n");
}

void find_node(Node node, link_t link_index) {
    Node temp;

    for (link_t i = 0; i < NODE_ABC; i++) {
        if (node[i] != 0) {
            if (link_index == LINKS_LEN - 1) {
                break;
            }
            fseek(tdb, node[i], SEEK_SET);
            fread(temp, sizeof(Node), 1, tdb);

            printf("%d-%d.", i, link_index);
            print_node(temp);
            find_node(temp, link_index+1);
        }
    }
}



void print_links(Links links) {
    printf("link: ");

    for (link_t i = 0; i < LINKS_LEN; i++)
        printf("%c%d", i && i % 3 == 0 ? '-' : 0, links[i]);

    printf("\n");
}

// char xx-xxx-xxxx to index
void phone_to_links(char *phone, Links links) {
    for (link_t i = 0; i < LINKS_LEN; i++)
        links[i] = phone[i] - '0';
}

void update(Links links, user_id_t value) {
    // Node *temp = malloc(sizeof(Node));
    Node temp;
    // link_t
    size_t pos = roots[links[0]][links[1]];
    size_t last_pos = pos;

    // roots[links[0]]

    // temp = roots[links[0]];

    // memcpy(temp, roots[links[0]], sizeof(Node));
    // memset(roots[links[0]], temp, sizeof(Node));

    // printf("temp: %ld\n", );

    for (link_t i = 1; i < LINKS_LEN; i++) {
        printf(
            "pos: %ld | i: %d | tdb_size: %ld | last_pos: %ld\n",
            pos, i, tdb_size, last_pos
        );


        if (pos == 0) {
            if (i == 1) {
                roots[links[0]][links[1]] = tdb_size;
            } else {
                fseek(tdb, last_pos, SEEK_SET);
                if (i == LINKS_LEN - 1)
                    temp[links[i]] = value;
                else
                    temp[links[i]] = tdb_size;

                fwrite(temp, sizeof(Node), 1, tdb);
            }

            pos = tdb_size;

            // empty the temp
            for (link_t i = 0; i < NODE_ABC; i++)
                temp[i] = 0;

            fseek(tdb, 0, SEEK_END);
            fwrite(temp, sizeof(Node), 1, tdb);
            tdb_size += sizeof(Node);
        }

        fseek(tdb, pos, SEEK_SET);
        fread(temp, sizeof(Node), 1, tdb);

        last_pos = pos;
        pos = temp[links[i]];
    }
}

// load or write the roots
void setup_roots(void) {
    // check if the database is empty or not
    // if its not empty load the roots and return
    fseek(tdb, 0, SEEK_END);
    tdb_size = ftell(tdb);
    // printf("tdb_size: %ld\n", tdb_size);

    // reset the file seek
    fseek(tdb, 0, SEEK_SET);

    if (tdb_size > 0) {
        fread(roots, sizeof(Node), NODE_ABC, tdb);
        return;
    }

    for (link_t i = 0; i < NODE_ABC; i++)
        for (link_t j = 0; j < NODE_ABC; j++)
            roots[i][j] = 0;

    fwrite(roots, sizeof(Node), NODE_ABC, tdb);
    tdb_size = sizeof(Node) * NODE_ABC;
}

*/

// print functions
void print_info(void);
void print_root(void);
void print_node(Node node);

// utils
Phone convert_phone(char *phone);

// node functions
void node_empty(Node node);
bool node_update(char *phone_number, user_id_t value);

// print functions
void print_info(void) {
    printf("------------INFO------------\n");
    printf("sizeof root :  %ld\n", sizeof(root));
    printf("cache level :  %d\n", CACHE_LVL);
    printf("cache size  :  %d\n", CACHE_SIZ);
    printf("----------------------------\n");
}

void print_root(void) {
    printf("-------------------------ROOT-------------------------\n");

    printf("     ");
    for (int i = 0; i < NODE_ABC; i++)
        printf("%d        ", i);

    printf("\n  0. ");

    for (int i = 0; i < CACHE_SIZ; i++) {
        if (i % 10 == 0 && i != 0)
            printf("\n%3d. ", i / 10);
        printf("%-8ld ", root[i]);
    }

    printf("\n------------------------------------------------------\n");
}

void print_node(Node node) {
    for (link_t i = 0; i < NODE_ABC; i++) {
        // if (i != 0) printf("|");
        printf(" %d:%-9ld", i, node[i]);
    }
    printf("\n");
}

// utils
Phone convert_phone(char *phone_number) {
    Phone phone;

    char cache_index[CACHE_LVL];
    strncpy(cache_index, phone_number, CACHE_LVL);

    phone.index = strtoul(cache_index, NULL, 10);

    for (link_t i = 0; i < LINKS_LEN; i++)
        phone.links[i] = phone_number[i + CACHE_LVL] - '0';

    return phone;
}

// node functions
void node_empty(Node node) {
    for (link_t i = 0; i < NODE_ABC; i++)
        node[i] = 0;
}

// add the node or update it
// return true if exists
bool node_update(char *phone_number, user_id_t value) {
    Phone phone = convert_phone(phone_number);
    size_t pos = root[phone.index];
    Node node;

    /*

    // debug
    printf("pos = root[%ld]: %ld in file\n", phone.index, pos);
    printf("links: ");
    for (link_t i = 0; i < LINKS_LEN; i++)
        printf("%d", phone.links[i]);
    printf("\nvalue: %ld\n----------------------------\n", value);
    // end debug

    */

    for (link_t i = 0; i < LINKS_LEN; i++) {
        fseek(phdb, pos, SEEK_SET);
        fread(node, sizeof(Node), 1, phdb);

        // printf("%-4ld->", pos);
        // print_node(node);

        if (node[phone.links[i]] == 0) {
            // printf("append ...\n");
            // read the file size (last byte position)
            size_t end_of_file = fsize(phdb);
            // go back to this node for updating it
            fseek(phdb, pos, SEEK_SET);
            // update the node the node
            node[phone.links[i]] = end_of_file;
            // write the updated node
            fwrite(node, sizeof(Node), 1, phdb);

            // printf("start: %ld\n", start);

            // go to the end of the file for appending to it
            fseek(phdb, 0, SEEK_END);

            // printf("%d|%d|%-4ld->", i, phone.links[i], pos);
            // print_node(node);

            // empty the node
            node_empty(node);

            // go forward once
            i++;

            for (; i < LINKS_LEN; i++) {
                // printf("%d|%d|%-4ld->", i, phone.links[i], end_of_file);

                end_of_file += sizeof(Node);

                if (i == LINKS_LEN - 1)
                    node[phone.links[i]] = value;
                else
                    node[phone.links[i]] = end_of_file;

                // print_node(node);

                fwrite(node, sizeof(Node), 1, phdb);
                node_empty(node);

                // printf("append: %d\n", i);
            }

            return false;
        }

        if (i == LINKS_LEN - 1) {
            bool exists = node[phone.links[i]] != 0;
            // printf("-----------------------------------\n");
            // printf("%-4ld->", pos);
            // print_node(node);

            node[phone.links[i]] = value;

            // printf("%-4ld->", pos);
            // print_node(node);

            fseek(phdb, pos, SEEK_SET);
            fwrite(node, sizeof(Node), 1, phdb);

            return exists;
        }

        pos = node[phone.links[i]];
    }

    return true;
}

// search for a node
user_id_t node_search(char *phone_number) {
    Phone phone = convert_phone(phone_number);
    size_t pos = root[phone.index];
    Node node;

    /*

    // debug
    printf("pos = root[%ld]: %ld in file\n", phone.index, pos);
    printf("links: ");
    for (link_t i = 0; i < LINKS_LEN; i++)
        printf("%d", phone.links[i]);
    printf("\n");
    // end debug

    */

    for (link_t i = 0; i < LINKS_LEN; i++) {
        fseek(phdb, pos, SEEK_SET);
        fread(node, sizeof(Node), 1, phdb);
        // printf("%-4ld->", pos);
        // print_node(node);

        if (node[phone.links[i]] == 0) {
            // printf("phone not found!\n");
            return 0;
        }

        if (i == LINKS_LEN - 1) {
            // printf("node[links[i]]: %ld\n", node[phone.links[i]]);
            return node[phone.links[i]];
        }

        /*
        if (node[links[i]] == 0) {
            printf("append ...\n");
            // read the file size (last byte position)
            size_t end_of_file = fsize(phdb);
            // go back to this node for updating it
            fseek(phdb, pos, SEEK_SET);
            // update the node the node
            node[links[i]] = end_of_file;
            // write the updated node
            fwrite(node, sizeof(Node), 1, phdb);

            // printf("start: %ld\n", start);

            // go to the end of the file for appending to it
            fseek(phdb, 0, SEEK_END);

            printf("%d|%d|%-4ld->", i, links[i], pos);
            print_node(node);

            // empty the node
            node_empty(node);

            // go forward once
            i++;

            for (; i < LINKS_LEN; i++) {

                printf("%d|%d|%-4ld->", i, links[i], end_of_file);


                end_of_file += sizeof(Node);

                if (i == LINKS_LEN - 1)
                    node[links[i]] = value;
                else
                    node[links[i]] = end_of_file;

                print_node(node);

                fwrite(node, sizeof(Node), 1, phdb);
                node_empty(node);

                // printf("append: %d\n", i);
            }

            return false;
        }
        */

        pos = node[phone.links[i]];
    }

    return 0;
}

void setup_root(void) {
    long pidb_size = fsize(pidb);
    long phdb_size = fsize(phdb);

    fseek(phdb, 0, SEEK_SET);
    fseek(pidb, 0, SEEK_SET);

    if (pidb_size < 0 || phdb_size < 0) {
        die("Error getting phone database size");
        return;
    }

    if (pidb_size == 0 && phdb_size == 0) {
        Node temp;
        node_empty(temp);

        for (size_t i = 0; i < CACHE_SIZ; i++) {
            root[i] = i * sizeof(Node);
            fwrite(&root[i], sizeof(size_t), 1, pidb);
            fwrite(temp, sizeof(Node), 1, phdb);
        }

        return;
    }

    if (pidb_size == sizeof(size_t) * CACHE_SIZ &&
        (size_t)phdb_size >= sizeof(Node) * CACHE_SIZ) {
        fread(root, sizeof(size_t), CACHE_SIZ, pidb);
        return;
    }

    die("error loading the cache");
}

void trie(void) {
    print_info();
    setup_root();

    /*

    clock_t begin = clock();


    // 219267107 908834300
    // 744789114 001454633
    // 824810336 308247332
    // 931613555 583354822
    // 226621675 827004940
    // 183053716 392638873
    // 560779926 688025154
    // 158185227 100162113
    // 790419624 427592638
    // 346583005 088118677


    printf("0921 926 7107 I: %d\n", node_update("219267107", 908834300));
    printf("0974 478 9114 I: %d\n", node_update("744789114", 991454633));
    printf("0982 481 0336 I: %d\n", node_update("824810336", 308247332));
    printf("0993 161 3555 I: %d\n", node_update("931613555", 583354822));
    printf("0922 662 1675 I: %d\n", node_update("226621675", 827004940));
    printf("0918 305 3716 I: %d\n", node_update("183053716", 392638873));
    printf("0956 077 9926 I: %d\n", node_update("560779926", 688025154));
    printf("0915 818 5227 I: %d\n", node_update("158185227", 100162113));
    printf("0979 041 9624 I: %d\n", node_update("790419624", 427592638));
    printf("0934 658 3005 I: %d\n", node_update("346583005", 688118677));


    printf("X969 359 0582 search: %ld\n", node_search("693590582"));
    printf("0922 662 1675 search: %ld\n", node_search("226621675"));
    printf("X920 737 0017 search: %ld\n", node_search("207370017"));
    printf("0934 658 3005 search: %ld\n", node_search("346583005"));
    printf("X912 893 5770 search: %ld\n", node_search("128935770"));


    printf("0915 818 5227 D: %d\n", node_update("158185227", 0));
    printf("X915 818 5227 S: %ld\n", node_search("158185227"));


    clock_t end = clock();
    printf("speed: %ld\n", end - begin);

    */
}

/*


insert 09101122222 - 69
insert 09104477777 - 89
insert 09998866666 - 124

search 09999999999
search 09998866666
search 09101122222

delete 09191122222

search 09181122222


the last node will point to user_id

Node: array of long 10x 8 bytes
-----------------
child 0 NULL or position in file (pointer but in file)
child 1 NULL or position in file (pointer but in file)
...
child 9 NULL or position in file (pointer but in file)
-----------------
-----------------
child 0 NULL or position in file (pointer but in file)
child 1 NULL or position in file (pointer but in file)
...
child 9 NULL or position in file (pointer but in file)
-----------------

*/
