
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../plutus.h"

// digits 0-9
#define NODE_ABC 10

// this two compine must be 9
// 09xx-xxx-xxxx
#define CACHE_LVL 3
// cache size is 10 ** CACHE_LVL
// 10 x 10 x 10
#define CACHE_SIZ 1000
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

void print_trie(void) {
    printf("-------------------------ROOT-------------------------\n");
    for (link_t i = 0; i < NODE_ABC; i++) {
        printf("%d.", i);
        print_node(roots[i]);
    }
    printf("------------------------------------------------------\n");

    for (link_t i = 0; i < NODE_ABC; i++) {
        find_node(roots[i], 1);
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
        if (i % 10 == 0 && i != 0) printf("\n%3d. ", i / 10);
        printf("%-8ld ", root[i]);
    }

    printf("\n------------------------------------------------------\n");
}


void node_empty(Node node) {
    for (link_t i = 0; i < NODE_ABC; i++)
        node[i] = 0;
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

    if (
        pidb_size == sizeof(size_t) * CACHE_SIZ && 
        (size_t)phdb_size >= sizeof(Node) * CACHE_SIZ
    ) {
        fread(root, sizeof(size_t), CACHE_SIZ, pidb);
        return;
    }

    die("error loading the cache");
}


void trie(void) {

    print_info();
    setup_root();
    // free(root);
    // printf("cache: %d\n", 10 ^ CACHE_LEV);
    // printf("links: %d\n", LINKS_LEN);
    // Links links;

    // setup_roots();

    // node_insert(&root, index, length-1, 79997);
    // phone_to_links("998836969", links);
    // print_links(links);
    // update(links, 696969);
    // print_trie();

    // node_insert(&root, index, length, 88888);

    // phone_to_index("187779955", index, &length);
    // node_insert(&root, index, length, 3333);

    // Node *child = malloc(sizeof(Node));
    // Node *temp  = &root;
    // // temp = &root;

    // printf("root.user_id: %ld\n", root.user_id);
    // printf("temp.user_id: %ld\n", temp->user_id);

    // for (int i = 0; i < length; i++) {
    //     child->user_id = i * 2 + i;
    //     temp->children[index[i]] = child;
    //     temp = child;

    //     child = malloc(sizeof(Node));
    // }

    // printf("----------------------------\n");
    // print_trie();
    // printf("----------------------------\n");
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
