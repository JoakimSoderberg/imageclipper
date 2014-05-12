/** @file
 * Particle Filter
 *
 * Currently suppports only linear state transition model. 
 * Write up a function by yourself to supports nonlinear dynamics
 * such as Taylor series model and call your function instead of 
 * cvParticleTransition( p ). 
 * Other functions should not necessary be modified.
 *
 * The MIT License
 * 
 * Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CV_PARTICLE_INCLUDED
#define CV_PARTICLE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include <time.h>
#include "cvsetrow.h"
#include "cvsetcol.h"
#include "cvlogsum.h"

/******************************* Structures **********************************/

typedef struct CvParticle {
    // config
    int num_states;    // Number of tracking states, e.g., 4 if x, y, width, height
    int num_observes;  // Number of observation models, e.g., 2 if color model and shape model
    int num_particles; // Number of particles
    bool logprob;      // probs are log probabilities
    // transition
    CvMat* dynamics;   // num_states x num_states. Dynamics model.
    CvRNG  rng;        // Random seed
    CvMat* std;        // num_states x 1. Standard deviation for gaussian noise
                       // Set standard deviation == 0 for no noise
    CvMat* bound;      // num_states x 3 (lowerbound, upperbound, circular flag 0 or 1)
                       // Set lowerbound == upperbound to express no bound
    // particle states
    CvMat* particles;  // num_states x num_particles. linked with probs. 
    CvMat* probs;      // num_observes x num_particles. linked with particles.
    CvMat* particle_probs; // 1 x num_particles. marginalization respect to observation models
    CvMat* observe_probs;  // num_observes x 1.  marginalization respect to tracking states
} CvParticle;

/**************************** Function Prototypes ****************************/

CvParticle* cvCreateParticle( int num_states, int num_observes, int num_particles, bool logprob = false );
void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics );
void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std );
void cvParticleSetBound( CvParticle* p );
void cvParticleInit( CvParticle* p, const CvParticle* init = NULL );
void cvReleaseParticle( CvParticle** p );

void cvParticleResample( CvParticle* p );
void cvParticleTransition( CvParticle* p );

void cvParticleMarginalize( CvParticle* p );
void cvParticleNormalize( CvParticle* p );
int  cvParticleMaxParticle( const CvParticle* p );
void cvParticleBound( CvParticle* p );

void cvParticlePrint( const CvParticle* p, int p_id = -1 );

/*************************** Function Definitions ****************************/

/**
 * Print states of a particle
 *
 * @param particle
 * @param p_id      particle id
 */
void cvParticlePrint( const CvParticle*p, int p_id )
{
    if( p_id == -1 ) // print all
    {
        int n;
        for( n = 0; n < p->num_particles; n++ )
        {
            cvParticlePrint( p, n );
        }
    }
    else {
        int i, k = 0;
        for( i = 0; i < p->num_states; i++ )
        {
            printf( "%6.1f ", cvmGet( p->particles, i, p_id ) );
        }
        printf( "\n" );
        fflush( stdout );
    }
}

/**
 * Re-samples a set of particles according to their probs to produce a
 * new set of unweighted particles
 *
 * Simply copy, not uniform randomly selects
 *
 * @param particle
 */
void cvParticleResample( CvParticle* p )
{
    int i, j, np, k = 0;
    CvMat* particle, hdr;
    CvMat* new_particles = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    double prob;
    int max_loc;

    cvParticleMarginalize( p );
    cvParticleNormalize( p );

    k = 0;
    for( i = 0; i < p->num_particles; i++ )
    {
        particle = cvGetCol( p->particles, &hdr, i );
        prob = cvmGet( p->particle_probs, 0, i );
        prob = p->logprob ? exp( prob ) : prob;
        np = cvRound( prob * p->num_particles );
        for( j = 0; j < np; j++ )
        {
            cvSetCol( particle, new_particles, k++ );
            if( k == p->num_particles )
                goto exit;
        }
    }

    max_loc = cvParticleMaxParticle( p );
    particle = cvGetCol( p->particles, &hdr, max_loc );
    while( k < p->num_particles )
        cvSetCol( particle, new_particles, k++ );

exit:
    cvReleaseMat( &p->particles );
    p->particles = new_particles;
}

/**
 * Get id of the most probable particle
 *
 * @param particle
 * @return int
 */
int cvParticleMaxParticle( const CvParticle* p )
{
    double minval, maxval;
    CvPoint min_loc, max_loc;
    cvMinMaxLoc( p->particle_probs, &minval, &maxval, &min_loc, &max_loc );
    return max_loc.x;
}

/**
 * Create particle_probs, and observe_probs by marginalizing
 *
 * @param particle
 * @todo priors
 */
void cvParticleMarginalize( CvParticle* p )
{
    if( p->logprob )
    {
        int np, no;
        CvScalar logsum;
        CvMat *particle_prob, *observe_prob, hdr;
        // number of particles of the same state represents priors
        for( np = 0; np < p->num_particles; np++ )
        {
            particle_prob = cvGetCol( p->probs, &hdr, np );
            logsum = cvLogSum( particle_prob );
            cvmSet( p->particle_probs, 0, np, logsum.val[0] );
        }
        // @todo: priors
        for( no = 0; no < p->num_observes; no++ )
        {
            observe_prob = cvGetRow( p->probs, &hdr, no );
            logsum = cvLogSum( particle_prob );
            cvmSet( p->observe_probs, no, 0, logsum.val[0] );
        }
    }
    else
    {
        // number of particles of the same state represents priors
        cvReduce( p->probs, p->particle_probs, -1, CV_REDUCE_SUM );
        // @todo: priors
        cvReduce( p->probs, p->observe_probs, -1, CV_REDUCE_SUM );
    }
}

/**
 * Normalize particle_probs and observe_probs so they sum to 1.0
 *
 * @param particle
 */
void cvParticleNormalize( CvParticle* p )
{
    // normalize particle_probs
    if( p->logprob )
    {
        CvScalar logsum = cvLogSum( p->particle_probs );
        cvSubS( p->particle_probs, logsum, p->particle_probs );
    } 
    else
    {
        CvScalar sum = cvSum( p->particle_probs );
        cvScale( p->particle_probs, p->particle_probs, 1.0 / sum.val[0] );
    }

    // normalize observe_probs
    if( p->logprob )
    {
        CvScalar logsum = cvLogSum( p->observe_probs );
        cvSubS( p->observe_probs, logsum, p->observe_probs );
    } 
    else
    {
        CvScalar sum = cvSum( p->observe_probs );
        cvScale( p->observe_probs, p->observe_probs, 1.0 / sum.val[0] );
    }
}

/**
 * Bound particle states
 *
 * @param particle
 */
void cvParticleBound( CvParticle* p )
{
    int row, col;
    double lower, upper;
    bool circular;
    int depend;
    CvMat* stateparticles, hdr;
    float state;
    // @todo:     np.width   = (double)MAX( 2.0, MIN( maxX - 1 - x, width ) );
    for( row = 0; row < p->num_states; row++ )
    {
        lower = cvmGet( p->bound, row, 0 );
        upper = cvmGet( p->bound, row, 1 );
        circular = (bool) cvmGet( p->bound, row, 2 );
        if( lower == upper ) continue; // no bound flag
        if( circular ) {
            for( col = 0; col < p->num_particles; col++ ) {
                state = cvmGet( p->particles, row, col );
                state = state < lower ? state + upper : ( state >= upper ? state - upper : state );
                cvmSet( p->particles, row, col, state );
            }
        } else {
            stateparticles = cvGetRow( p->particles, &hdr, row );
            cvMinS( stateparticles, upper, stateparticles );
            cvMaxS( stateparticles, lower, stateparticles );
        }
    }
}

/**
 * Samples new particles from given particles
 *
 * Currently suppports only linear combination of states transition model. 
 * Write up a function by yourself to supports nonlinear dynamics
 * such as Taylor series model and call your function instead of this function. 
 * Other functions should not necessary be modified.
 *
 * @param particle
 */
void cvParticleTransition( CvParticle* p )
{
    int i, j;
    CvMat* transits = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noises   = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noise, noisehdr;
    double std;
    
    // dynamics
    cvMatMul( p->dynamics, p->particles, transits );
    
    // noise generation
    for( i = 0; i < p->num_states; i++ )
    {
        std    = cvmGet( p->std, i, 0 );
        noise = cvGetRow( noises, &noisehdr, i );
        if( std == 0.0 )
            cvZero( noise );
        else
            cvRandArr( &p->rng, noise, CV_RAND_NORMAL, cvScalar(0), cvScalar( std ) );
    }
    
    // dynamics + noise
    cvAdd( transits, noises, p->particles );

    cvReleaseMat( &transits );
    cvReleaseMat( &noises );

    cvParticleBound( p );
}

/**
 * Initialize states
 *
 * @param particle
 * @param [init = NULL] Manually tell initial states. 
 *                      Use num_particles as number of initial candidates for this.
 *                      If not given, uniform randomly initialize
 */
void cvParticleInit( CvParticle* p, const CvParticle* init )
{
    int i, j, k;
    if( init )
    {
        int *num_copy;
        CvMat init_state;

        int divide = p->num_particles / init->num_particles;
        int remain = p->num_particles - divide * init->num_particles;
        num_copy = (int*) malloc( init->num_particles * sizeof(int) );
        for( i = 0; i < init->num_particles; i++ )
        {
            num_copy[i] = divide + ( i < remain ? 1 : 0 );
        }
        
        k = 0;
        for( i = 0; i < init->num_particles; i++ )
        {
            cvGetCol( init->particles, &init_state, i );
            for( j = 0; j < num_copy[i]; j++ )
            {
                cvSetCol( &init_state, p->particles, k++ );
            }
        }

        free( num_copy );
    } 
    else
    {
        CvScalar lower, upper;
        CvMat* onenoiseT = cvCreateMat( p->num_particles, 1, p->particles->type );
        CvMat* onenoise  = cvCreateMat( 1, p->num_particles, p->particles->type );
        for( i = 0; i < p->num_states; i++ )
        {
            lower = cvScalar( cvmGet( p->bound, i, 0 ) );
            upper = cvScalar( cvmGet( p->bound, i, 1 ) );
            cvRandArr( &p->rng, onenoiseT, CV_RAND_UNI, lower, upper );
            cvT( onenoiseT, onenoise );
            cvSetRow( onenoise, p->particles, i );
        }
        cvReleaseMat( &onenoise );
        cvReleaseMat( &onenoiseT );
    }
}

/**
 * Set lowerbound and upperbound for tracking states
 *
 * @param particle
 * @param bound    num_states x 3 (lowerbound, upperbound, circular flag 0 or 1)
 *                 Set lowerbound == upperbound to express no bound
 */
void cvParticleSetBound( CvParticle* p, const CvMat* bound )
{
    CV_FUNCNAME( "cvParticleSetBound" );
    __BEGIN__;
    CV_ASSERT( p->num_states == bound->rows );
    CV_ASSERT( 3 == bound->cols );
    //cvCopy( bound, p->bound );
    cvConvert( bound, p->bound );
    __END__;
}

/**
 * Set noise model
 *
 * @param particle
 * @param rng      random seed. refer cvRNG(time(NULL))
 * @param std      num_states x 1. standard deviation for gaussian noise
 *                 Set standard deviation == 0 for no noise
 */
void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std )
{
    CV_FUNCNAME( "cvParticleSetNoise" );
    __BEGIN__;
    CV_ASSERT( p->num_states == std->rows );
    p->rng = rng;
    //cvCopy( std, p->std );
    cvConvert( std, p->std );
    __END__;
}

/**
 * Set dynamics model
 *
 * @param particle
 * @param dynamics (num_states) x (num_states). dynamics model
 *    new_state = dynamics * curr_state + noise
 */
void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics )
{
    CV_FUNCNAME( "cvParticleSetDynamics" );
    __BEGIN__;
    CV_ASSERT( p->num_states == dynamics->rows );
    CV_ASSERT( p->num_states == dynamics->cols );
    //cvCopy( dynamics, p->dynamics );
    cvConvert( dynamics, p->dynamics );
    __END__;
}

/**
 * Release Particle filter structure
 *
 * @param particle
 * @return void
 */
void cvReleaseParticle( CvParticle** particle )
{
    CvParticle *p = NULL;
    CV_FUNCNAME( "cvReleaseParticle" );
    __BEGIN__;
    p = *particle;
    if( !p ) EXIT;
    
    cvReleaseMat( &p->dynamics );
    cvReleaseMat( &p->std );
    cvReleaseMat( &p->bound );
    cvReleaseMat( &p->particles );
    cvReleaseMat( &p->probs );
    cvFree( &p );
    __END__;
}

/**
 * Allocate Particle filter structure
 *
 * @param num_states    Number of tracking states, e.g., 4 if x, y, width, height
 * @param num_observes  Number of observation models, e.g., 2 if color model and shape model
 * @param num_particles Number of particles
 * @param [logprob = false]
 *                      The probs parameter is log probabilities or not
 * @return CvParticle*
 */
CvParticle* cvCreateParticle( int num_states, int num_observes, int num_particles, bool logprob )
{
    CvParticle *p = NULL;
    CV_FUNCNAME( "cvCreateParticle" );
    __BEGIN__;
    CV_ASSERT( num_states > 0 );
    CV_ASSERT( num_observes > 0 );
    CV_ASSERT( num_particles > 0 );
    p = (CvParticle *) cvAlloc( sizeof( CvParticle ) );
    p->num_particles = num_particles;
    p->num_states    = num_states;
    p->num_observes  = num_observes;
    p->dynamics      = cvCreateMat( num_states, num_states, CV_32FC1 );
    p->rng           = 1;
    p->std           = cvCreateMat( num_states, 1, CV_32FC1 );
    p->bound         = cvCreateMat( num_states, 3, CV_32FC1 );
    p->particles     = cvCreateMat( num_states, num_particles, CV_32FC1 );
    p->probs         = cvCreateMat( num_observes, num_particles, CV_64FC1 );
    p->particle_probs = cvCreateMat( 1, num_particles, CV_64FC1 );
    p->observe_probs  = cvCreateMat( num_observes, 1, CV_64FC1 );
    p->logprob        = logprob;

    // Default dynamics: next state = curr state + noise
    cvSetIdentity( p->dynamics, cvScalar(1.0) );
    cvSet( p->std, cvScalar(1.0) );

    cvZero( p->bound );

    __END__;
    return p;
}


#endif
