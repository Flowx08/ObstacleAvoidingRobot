#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RANDF() (double)rand() / RAND_MAX 

typedef struct Qstruct
{
	float **table;
	int states_count;
	int actions_count;
	int state;
	int action;
	float learning_rate;
	float longterm_learning;
        float exploration_factor;
} Qstruct;

Qstruct* qstruct_init(int states_count, int actions_count)
{
	Qstruct* q = (Qstruct*)malloc(sizeof(Qstruct));
	q->table = (float**)malloc(sizeof(float*) * states_count);
	for (int i = 0; i < states_count; i++) {
		q->table[i] = (float*)malloc(sizeof(float) * actions_count);
		for (int k = 0; k < actions_count; k++)
			q->table[i][k] = 0;
	}
	q->states_count = states_count;
	q->actions_count = actions_count;
	q->learning_rate = 0.5;
	q->longterm_learning = 0.5;
        q->exploration_factor = 0.1;
	return q;
}

void qstruct_free(Qstruct* q)
{
	for (int i = 0; i < q->states_count; i++)
		free(q->table[i]);
	free(q->table);
	free(q);
}

int qstruct_takeaction(Qstruct* q, int state)
{
	if (RANDF() < q->exploration_factor) {
                q->state = state;
		q->action = rand() % q->actions_count;
		return q->action;
	}
	double rmax = -0x7FFF;
	int maxid = 0;
	for (int i = 0; i < q->actions_count; i++)
		if (q->table[state][i] > rmax) {
			rmax = q->table[state][i];
			maxid = i;
		}
	q->state = state;
	q->action = maxid;
	return q->action;
}

double qstruct_getmaxreward(Qstruct* q, int state)
{
	float rmax = -0x7FFF;
	for (int i = 0; i < q->actions_count; i++)
		if (q->table[state][i] > rmax) rmax = q->table[state][i];
	return rmax;
}

void qstruct_update(Qstruct* q, int state_new, float reward)
{
	q->table[q->state][q->action] += q->learning_rate * (reward +  
		q->longterm_learning * qstruct_getmaxreward(q, state_new) - q->table[q->state][q->action]);
}

#ifdef __cplusplus
}
#endif

