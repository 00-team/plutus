
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "plutus.h"
#include "phone.h"
#include "logger.h"

#define LOG_SCTOR SECTOR_USER


int phdb = -1;

void print_node(Node node) {
    for (link_t i = 0; i < NODE_ABC; i++) {
        // if (i != 0) printf("|");
        printf(" %d:%-9ld", i, node[i]);
    }
    printf("\n");
}

void print_phone(Phone *phone) {
    printf("------- PHONE -------\n");
    printf("index: %ld\nlinks: ", phone->index);
    for (link_t i = 0; i < LINKS_LEN; i++) {
        // if (i != 0) printf("|");
        printf("%d", phone->links[i]);
    }
    printf("\n---------------------\n");
}


// node
bool node_write(Node node) {
    return obj_write(phdb, node, sizeof(Node), NULL);
}

bool node_read(Node node) {
    return obj_read(phdb, node, sizeof(Node), NULL);
}

// convert a phone number string
// to root index and links
Phone phone_convert(char *phone_number) {
    Phone phone;

    // seek the fist 2 digits 09xx
    phone_number++;
    phone_number++;

    char cache_index[CACHE_LVL];
    strncpy(cache_index, phone_number, CACHE_LVL);

    phone.index = strtoul(cache_index, NULL, 10);

    for (link_t i = 0; i < LINKS_LEN; i++)
        phone.links[i] = phone_number[i + CACHE_LVL] - '0';

    return phone;
}


// add the node or update it
// return true if exists
bool phone_update(Phone *phone, user_id_t value) {
    size_t pos = phone->index * sizeof(Node);
    Node node;
    link_t link = 0;

    for (link_t i = 0; i < LINKS_LEN; i++) {
        link = phone->links[i];
        lseek(phdb, pos, SEEK_SET);
        node_read(node);

        // check of the current node is empty or not
        // if it is then we will write rest of the nodes
        if (node[link] == 0) {
            // read the file size (last byte position)
            off_t end_of_file = seek_append(phdb, sizeof(Node));

            // go back to the current node for updating it
            lseek(phdb, pos, SEEK_SET);

            // set the next imaginary node position to the current one
            if (i == LINKS_LEN - 1)
                node[link] = value;
            else
                node[link] = end_of_file;

            // write the updated node
            node_write(node);

            // go to the end of the file for appending to it
            lseek(phdb, end_of_file, SEEK_SET);

            // empty the node
            memset(node, 0, sizeof(Node));

            // loop over ramaning links for writing them
            for (i++; i < LINKS_LEN; i++) {
                link = phone->links[i];

                // every node after this will be written at the next block
                // so we just imagine that this will be the next link position
                end_of_file += sizeof(Node);

                // check if where are at the end of our links
                // then set the last value to user_id
                // if we are not set the value to the next link
                if (i == LINKS_LEN - 1)
                    node[link] = value;
                else
                    node[link] = end_of_file;

                node_write(node);
                memset(node, 0, sizeof(Node));
            }

            return false;
        }

        if (i == LINKS_LEN - 1) {
            bool exists = node[link] != 0;

            node[link] = value;

            lseek(phdb, pos, SEEK_SET);
            node_write(node);

            return exists;
        }

        pos = node[link];
    }

    return false;
}


// search for a node
user_id_t phone_search(Phone *phone) {
    size_t pos = phone->index * sizeof(Node);
    Node node;

    for (link_t i = 0; i < LINKS_LEN; i++) {
        lseek(phdb, pos, SEEK_SET);
        node_read(node);
        pos = node[phone->links[i]];

        if (pos == 0) return 0;
        if (i == LINKS_LEN - 1) return pos;
    }

    return 0;
}


void phone_setup(void) {
    off_t phdb_size = fsize(phdb);

    lseek(phdb, 0, SEEK_SET);

    if (phdb_size == 0) {
        Node temp;
        memset(temp, 0, sizeof(Node));

        for (size_t i = 0; i < CACHE_SIZ; i++) {
            node_write(temp);
        }

        log_info("setup finish");
        return;
    }

    if ((size_t)phdb_size < sizeof(Node) * CACHE_SIZ) {
        log_error("database setup %ld != %ld!", phdb_size, sizeof(Node) * CACHE_SIZ);
    }
}

