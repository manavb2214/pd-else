// matt barber and porres (2018)
// based on SuperCollider's white UGen

#include "m_pd.h"
#include "random.h"

static t_class *white_class;

typedef struct _white{
    t_object       x_obj;
    t_random_state x_rstate;
    t_outlet      *x_outlet;
}t_white;

static unsigned int instanc_n = 0;

static void white_seed(t_white *x, t_symbol *s, int ac, t_atom *av){
    s = NULL;
    unsigned int timeval;
    if(ac && av->a_type == A_FLOAT)
        timeval = (unsigned int)(atom_getfloat(av));
    else
        timeval = (unsigned int)(time(NULL)*151*++instanc_n);
    random_init(&x->x_rstate, timeval);
}

static t_int *white_perform(t_int *w){
    int n = (t_int)(w[1]);
    t_random_state *rstate = (t_random_state *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    uint32_t *s1 = &rstate->s1;
    uint32_t *s2 = &rstate->s2;
    uint32_t *s3 = &rstate->s3;
    while(n--)
    	*out++ = (t_float)(random_frand(s1, s2, s3));
    return(w+5);
}

static void white_dsp(t_white *x, t_signal **sp){
    dsp_add(white_perform, 4, sp[0]->s_n, &x->x_rstate, sp[0]->s_vec);
}

static void *white_new(t_symbol *s, int ac, t_atom *av){
    t_white *x = (t_white *)pd_new(white_class);
    x->x_outlet = outlet_new(&x->x_obj, &s_signal);
    white_seed(x, s, ac, av);
    return(x);
}

void white_tilde_setup(void){
    white_class = class_new(gensym("white~"), (t_newmethod)white_new,
        0, sizeof(t_white), 0, A_GIMME, 0);
    class_addmethod(white_class, (t_method)white_dsp, gensym("dsp"), A_CANT, 0);
    class_addmethod(white_class, (t_method)white_seed, gensym("seed"), A_GIMME, 0);
}