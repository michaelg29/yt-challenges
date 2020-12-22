#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minheap.h"

typedef struct gift
{
    float price;
    char giver[50];
    char receiver[50];
} gift;

int giftcompare(void *g1, void *g2)
{
    gift *gift1 = (gift *)g1;
    gift *gift2 = (gift *)g2;

    if (gift1->price > gift2->price)
    {
        return 1;
    }
    else if (gift1->price == gift2->price)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

void printgift(void *g)
{
    gift *giftVal = (gift *)g;

    printf("%s gave %s a gift worth %f\n", giftVal->giver, giftVal->receiver, giftVal->price);
}

int main()
{
    minheap h = mheap_allocate(giftcompare);

    while (1)
    {
        gift *newGift = malloc(sizeof(gift));

        printf("From: ");
        scanf("%s", newGift->giver);

        if (!strcmp(newGift->giver, "q"))
        {
            break;
        }

        printf("To: ");
        scanf("%s", newGift->receiver);

        printf("Price: ");
        scanf("%f", &newGift->price);

        printgift(newGift);

        mheap_add(&h, newGift);
    }

    printf("=================\nChristmas Morning\n\n");
    void *current = NULL;
    while (current = mheap_pop(&h))
    {
        printgift(current);
    }

    mheap_freeDeep(&h);
}