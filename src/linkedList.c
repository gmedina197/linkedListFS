typedef struct node
{
    unsigned short int number;
    struct node *next;
} node_t;

void print_list(node_t *head)
{
    node_t *current = head;

    while (current != NULL)
    {
        printf("%d\n", current->number);
        current = current->next;
    }
}

/* void push(node_t *head, int val)
{
    node_t *current = head;
    if (current != NULL)
    {
        while (current->next != NULL)
        {
            current = current->next;
        }

        current->next = malloc(sizeof(node_t));
        current->next->number = val;
        current->next->next = NULL;
    } else {
        head = malloc(sizeof(node_t));
        head->number = 1;
        head->next->next = NULL;
    }
} */

void push(node_t **head, int val)
{
    node_t *new_node;
    new_node = malloc(sizeof(node_t));

    new_node->number = val;
    new_node->next = *head;
    *head = new_node;
}

int pop(node_t **head)
{
    int retval = -1;
    node_t *next_node = NULL;

    if (*head == NULL)
    {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->number;
    free(*head);
    *head = next_node;

    return retval;
}