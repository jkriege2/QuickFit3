#ifndef LINALG3_H_INCLUDED
#define LINALG3_H_INCLUDED


/*!
    \defgroup tools_math_linalg3 3D Linear Algebra
    \ingroup tools_math_linalg

    \file linalg3.h
    \ingroup tools_math_linalg3
*/


/*! \brief represent a 3D vector of doubles with a large set of functions
    \ingroup tools_math_linalg3
 */
class jkVector3 {
    friend jkVector3 operator*(const double& a, const jkVector3& b);
    friend jkVector3 crossProduct(const jkVector3& a, const jkVector3& b);
    friend double dotProduct(const jkVector3& a, const jkVector3& b);
    public:
        /*! \brief constructor with initialisation \f$ (x, y, z)^t \f$

            \param x initial first component
            \param y initial second component
            \param z initial third component
        */
        jkVector3(double x, double y, double z) {
            m[0]=x;
            m[1]=y;
            m[2]=z;
        }

        /*! \brief copy constructor

            \param a copy data from this vector
        */
        jkVector3(const jkVector3& a ) {
            m[0]=a.m[0];
            m[1]=a.m[1];
            m[2]=a.m[2];
        }

        /*! \brief constructor with initialisation by 3D \c double array vector

            \param x initial vector
        */
        jkVector3(double* x) {
            m[0]=x[0];
            m[1]=x[1];
            m[2]=x[2];
        }

        /*! \brief constructor with initialisation \f$ (0, 0, 0)^t \f$ */
        jkVector3() {
            m[0]=m[1]=m[2]=0;
        }

        /** \brief copy vector into given \c double array
         *
         *  \param[out] a output array
         */
        void fill(double* a) {
            a[0]=m[0];
            a[1]=m[1];
            a[2]=m[2];
        }

        /*! \brief assignment operator to \f$ (a, a, a)^t \f$

            \param a set all components with this value
            \return reference to this object
        */
        jkVector3& operator=( const double a ) { //Defi nition of the copy assignment operator
            m[0]=a;
            m[1]=a;
            m[2]=a;
            return *this;
        }

        /*! \brief assignment operator

            \param a copy data from this vector
            \return reference to this object
        */
        jkVector3& operator=( const jkVector3& a ) { //Defi nition of the copy assignment operator
            if( this == &a ) //Self-protection against self-assignment
                return *this; //
            m[0]=a.m[0];
            m[1]=a.m[1];
            m[2]=a.m[2];
            return *this;
        }
        /** \brief return x/first component */
        inline double x() const { return m[0]; }

        /** \brief return y/second component */
        inline double y() const { return m[1]; }

        /** \brief return z/third component */
        inline double z() const { return m[2]; }

        /** \brief return L2 norm of vector \f$ |\vec{a}|_2=\sqrt{\vec{a}\cdot\vec{a}}=\sqrt{\sum\limits_{i=1}^3a_i^2} \f$ */
        inline double normL2() const { return sqrt(m[0]*m[0]+m[1]*m[1]+m[2]*m[2]); }

        /** \brief return L2 normalized vector \f$ \vec{a}/|\vec{a}|_2 \f$ */
        inline jkVector3 normalizedL2() const {
            const double l=sqrt(m[0]*m[0]+m[1]*m[1]+m[2]*m[2]);
            return jkVector3(m[0]/l, m[1]/l, m[2]/l);
        }

        /** \brief L2 normalize vector \f$ \vec{a}\leftarrow\vec{a}/|\vec{a}|_2 \f$
         *  \return reference to this vector
         */
        inline jkVector3& normalizeL2() {
            const double l=sqrt(m[0]*m[0]+m[1]*m[1]+m[2]*m[2]);
            m[0]=m[0]/l; m[1]=m[1]/l; m[2]=m[2]/l;
            return *this;
        }

        /** \brief return squared L2 norm of vector \f$ |\vec{a}|_2^2=\vec{a}\cdot\vec{a}=\sum\limits_{i=1}^3a_i^2 \f$ */
        inline double normSquareL2() const { return m[0]*m[0]+m[1]*m[1]+m[2]*m[2]; }

        /** \brief \copydoc normL2() */
        inline double length() const { return normL2(); }

        /*! \brief scalar product of current vector with a given vector
            \param b other vector
            \return scalar product \f$ \vec{a}\cdot\vec{b} \f$
        */
        inline double scalarProduct(const jkVector3& b) const {
            return m[0]*b.m[0]+m[1]*b.m[1]+m[2]*b.m[2];
        }

        /*! \brief sum up this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return sum \f$ \vec{a}+\vec{b} \f$
         */
        inline jkVector3 operator+(const jkVector3& b) const {
            return jkVector3(m[0]+b.m[0], m[1]+b.m[1], m[2]+b.m[2]);
        }
        /*! \brief subtract this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return difference \f$ \vec{a}-\vec{b} \f$
         */
        inline jkVector3 operator-(const jkVector3& b) const {
            return jkVector3(m[0]-b.m[0], m[1]-b.m[1], m[2]-b.m[2]);
        }
        /*! \brief component-wise multiply this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return \f$ \begin{pmatrix}a_1b_1\\a_2b_2\\a_3b_3\end{pmatrix} \f$
         */
        inline jkVector3 operator*(const jkVector3& b) const {
            return jkVector3(m[0]*b.m[0], m[1]*b.m[1], m[2]*b.m[2]);
        }
        /*! \brief component-wise divide this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return \f$ \begin{pmatrix}a_1/b_1\\a_2/b_2\\a_3/b_3\end{pmatrix} \f$
         */
        inline jkVector3 operator/(const jkVector3& b) const {
            return jkVector3(m[0]/b.m[0], m[1]/b.m[1], m[2]/b.m[2]);
        }

        /*! \brief sum assign operator \f$ \vec{a}\leftarrow\vec{a}+\vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector3& operator+=(const jkVector3& b) {
            m[0]=m[0]+b.m[0]; m[1]=m[1]+b.m[1]; m[2]=m[2]+b.m[2];
            return *this;
        }
        /*! \brief subtract assign operator \f$ \vec{a}\leftarrow\vec{a}-\vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector3& operator-=(const jkVector3& b) {
            m[0]=m[0]-b.m[0]; m[1]=m[1]-b.m[1]; m[2]=m[2]-b.m[2];
            return *this;
        }
        /*! \brief multiply assign operator \f$ a_i\leftarrow a_i\cdot b_i \f$ (component-wise)
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector3& operator*=(const jkVector3& b) {
            m[0]=m[0]*b.m[0]; m[1]=m[1]*b.m[1]; m[2]=m[2]*b.m[2];
            return *this;
        }
        /*! \brief divide assign operator \f$ a_i\leftarrow a_i/b_i \f$ (component-wise)
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector3& operator/=(const jkVector3& b) {
            m[0]=m[0]/b.m[0]; m[1]=m[1]/b.m[1]; m[2]=m[2]/b.m[2];
            return *this;
        }

        /*! \brief sum up each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i+b \f$
         */
        inline jkVector3 operator+(const double& b) const {
            return jkVector3(m[0]+b, m[1]+b, m[2]+b);
        }
        /*! \brief subtract each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i-b \f$
         */
        inline jkVector3 operator-(const double& b) const {
            return jkVector3(m[0]-b, m[1]-b, m[2]-b);
        }
        /*! \brief multiply each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i\cdot b \f$
         */
        inline jkVector3 operator*(const double& b) const {
            return jkVector3(m[0]*b, m[1]*b, m[2]*b);
        }
        /*! \brief diide each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i/b \f$
         */
        inline jkVector3 operator/(const double& b) const {
            return jkVector3(m[0]/b, m[1]/b, m[2]/b);
        }

        /*! \brief sum assign operator \f$ a_i\leftarrow a_i+b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector3& operator+=(const double& b) {
            m[0]=m[0]+b; m[1]=m[1]+b; m[2]=m[2]+b;
            return *this;
        }
        /*! \brief subtract assign operator \f$ a_i\leftarrow a_i-b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector3& operator-=(const double& b) {
            m[0]=m[0]-b; m[1]=m[1]-b; m[2]=m[2]-b;
            return *this;
        }
        /*! \brief multiply assign operator \f$ a_i\leftarrow a_i\cdot b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector3& operator*=(const double& b) {
            m[0]=m[0]*b; m[1]=m[1]*b; m[2]=m[2]*b;
            return *this;
        }
        /*! \brief divide assign operator \f$ a_i\leftarrow a_i/b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector3& operator/=(const double& b) {
            m[0]=m[0]/b; m[1]=m[1]/b; m[2]=m[2]/b;
            return *this;
        }

        /** \brief return pointer to data array (3 components of type \c double) */
        double* data() { return m; }


    protected:

        double m[3];
};




/*! \brief dot/scalar product of two vectors \f$ \vec{a}\cdot\vec{b} \f$
    \ingroup tools_math_linalg3

    \param a first input vector
    \param b second input vector
    \return dot/scalar product \f[ \vec{a} \cdot \vec{b}=\sum\limits_{i=1}^3a_ib_i \f]
*/
inline double dotProduct(const jkVector3& a, const jkVector3& b) {
    return a.m[0]*b.m[0]+a.m[1]*b.m[1]+a.m[2]*b.m[2];
}

/*! \brief multiply a 3D vector with a number (scale the vector)
    \ingroup tools_math_linalg3

    \param a number to multiply with \f$ a \f$
    \param b vector \f$ \vec{b} \f$
    \return \f$ a\cdot\vec{b} \f$
*/
inline jkVector3 operator*(const double& a, const jkVector3& b) {
    return jkVector3(a*b.m[0], a*b.m[1], a*b.m[2]);
}

/*! \brief calculate the cross product of two 3D vectors \f$ \vec{a} \times \vec{b} \f$
    \ingroup tools_math_linalg3

    \param a first input vector
    \param b second input vector
    \return cross product \f[ \vec{a}\times\vec{b} = \begin{pmatrix} a_2b_3-a_3b_2\\ a_3b_1-a_1b_3\\ a_1b_2-a_2b_1 \end{pmatrix} \f]

*/
inline jkVector3 crossProduct(const jkVector3& a, const jkVector3& b) {
    return jkVector3(a.m[1]*b.m[2]-a.m[2]*b.m[1], a.m[2]*b.m[0]-a.m[0]*b.m[2], a.m[0]*b.m[1]-a.m[1]*b.m[0]);
}

#endif // LINALG3_H_INCLUDED
