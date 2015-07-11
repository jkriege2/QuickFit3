#ifndef LINALG2_H_INCLUDED
#define LINALG2_H_INCLUDED

/*!
    \defgroup tools_math_linalg2 2D Linear Algebra
    \ingroup tools_math_linalg

    \file linalg2.h
    \ingroup tools_math_linalg2
*/

/*! \brief represent a 2D vector of doubles with a large set of functions
    \ingroup tools_math_linalg2
 */
class jkVector2 {
    friend jkVector2 operator*(const double& a, const jkVector2& b);
    friend double dotProduct(const jkVector2& a, const jkVector2& b);
    public:
        /*! \brief constructor with initialisation \f$ (x, y)^t \f$

            \param x initial first component
            \param y initial second component
        */
        jkVector2(double x, double y) {
            m[0]=x;
            m[1]=y;
        }

        /*! \brief copy constructor
            \param a copy data from this vector
        */
        jkVector2(const jkVector2& a ) {
            m[0]=a.m[0];
            m[1]=a.m[1];
        }

        /*! \brief constructor with initialisation by 2D \c double array vector
            \param x initial vector
        */
        jkVector2(double* x) {
            m[0]=x[0];
            m[1]=x[1];
        }

        /*! \brief constructor with initialisation \f$ (0, 0)^t \f$ */
        jkVector2() {
            m[0]=m[1]=0;
        }

        /** \brief copy vector into given \c double array
         *
         *  \param[out] a output array
         */
        void fill(double* a) {
            a[0]=m[0];
            a[1]=m[1];
        }

        double& operator[](int i) {
            if (i==1) return m[1];
            return m[0];
        }

        /*! \brief assignment operator to \f$ (a, a)^t \f$
            \param a set all components with this value
            \return reference to this object
        */
        jkVector2& operator=( const double a ) { //Defi nition of the copy assignment operator
            m[0]=a;
            m[1]=a;
            return *this;
        }

        /*! \brief assignment operator

            \param a copy data from this vector
            \return reference to this object
        */
        jkVector2& operator=( const jkVector2& a ) { //Defi nition of the copy assignment operator
            if( this == &a ) //Self-protection against self-assignment
                return *this; //
            m[0]=a.m[0];
            m[1]=a.m[1];
            return *this;
        }
        /** \brief return x/first component */
        inline double x() const { return m[0]; }

        /** \brief return y/second component */
        inline double y() const { return m[1]; }


        /** \brief return L2 norm of vector \f$ |\vec{a}|_2=\sqrt{\vec{a}\cdot\vec{a}}=\sqrt{a_1^2+a_2^2} \f$ */
        inline double normL2() const { return sqrt(m[0]*m[0]+m[1]*m[1]); }

        /** \brief return L2 normalized vector \f$ \vec{a}/|\vec{a}|_2 \f$ */
        inline jkVector2 normalizedL2() const {
            const double l=sqrt(m[0]*m[0]+m[1]*m[1]);
            return jkVector2(m[0]/l, m[1]/l);
        }

        /** \brief L2 normalize vector \f$ \vec{a}\leftarrow\vec{a}/|\vec{a}|_2 \f$
         *  \return reference to this vector
         */
        inline jkVector2& normalizeL2() {
            const double l=sqrt(m[0]*m[0]+m[1]*m[1]);
            m[0]=m[0]/l; m[1]=m[1]/l;
            return *this;
        }

        /** \brief return squared L2 norm of vector \f$ |\vec{a}|_2^2=\vec{a}\cdot\vec{a}=a_1^2+a_2^2 \f$ */
        inline double normSquareL2() const { return m[0]*m[0]+m[1]*m[1]; }

        /** \brief \copydoc normL2() */
        inline double length() const { return normL2(); }

        /*! \brief scalar product of current vector with a given vector
            \param b other vector
            \return scalar product \f$ \vec{a}\cdot\vec{b} \f$
        */
        inline double scalarProduct(const jkVector2& b) const {
            return m[0]*b.m[0]+m[1]*b.m[1];
        }

        /*! \brief sum up this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return sum \f$ \vec{a}+\vec{b} \f$
         */
        inline jkVector2 operator+(const jkVector2& b) const {
            return jkVector2(m[0]+b.m[0], m[1]+b.m[1]);
        }
        /*! \brief subtract this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return difference \f$ \vec{a}-\vec{b} \f$
         */
        inline jkVector2 operator-(const jkVector2& b) const {
            return jkVector2(m[0]-b.m[0], m[1]-b.m[1]);
        }
        /*! \brief component-wise multiply this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return \f$ \begin{pmatrix}a_1b_1\\a_2b_2\end{pmatrix} \f$
         */
        inline jkVector2 operator*(const jkVector2& b) const {
            return jkVector2(m[0]*b.m[0], m[1]*b.m[1]);
        }
        /*! \brief component-wise divide this vectors \f$ \vec{a} \f$ and a second vector \f$ \vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return \f$ \begin{pmatrix}a_1/b_1\\a_2/b_2\end{pmatrix} \f$
         */
        inline jkVector2 operator/(const jkVector2& b) const {
            return jkVector2(m[0]/b.m[0], m[1]/b.m[1]);
        }

        /*! \brief sum assign operator \f$ \vec{a}\leftarrow\vec{a}+\vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector2& operator+=(const jkVector2& b) {
            m[0]=m[0]+b.m[0]; m[1]=m[1]+b.m[1];
            return *this;
        }
        /*! \brief subtract assign operator \f$ \vec{a}\leftarrow\vec{a}-\vec{b} \f$
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector2& operator-=(const jkVector2& b) {
            m[0]=m[0]-b.m[0]; m[1]=m[1]-b.m[1];
            return *this;
        }
        /*! \brief multiply assign operator \f$ a_i\leftarrow a_i\cdot b_i \f$ (component-wise)
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector2& operator*=(const jkVector2& b) {
            m[0]=m[0]*b.m[0]; m[1]=m[1]*b.m[1];
            return *this;
        }
        /*! \brief divide assign operator \f$ a_i\leftarrow a_i/b_i \f$ (component-wise)
            \param b second vector \f$ \vec{b} \f$
            \return reference to this vector
         */
        inline jkVector2& operator/=(const jkVector2& b) {
            m[0]=m[0]/b.m[0]; m[1]=m[1]/b.m[1];
            return *this;
        }

        /*! \brief sum up each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i+b \f$
         */
        inline jkVector2 operator+(const double& b) const {
            return jkVector2(m[0]+b, m[1]+b);
        }
        /*! \brief subtract each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i-b \f$
         */
        inline jkVector2 operator-(const double& b) const {
            return jkVector2(m[0]-b, m[1]-b);
        }
        /*! \brief multiply each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i\cdot b \f$
         */
        inline jkVector2 operator*(const double& b) const {
            return jkVector2(m[0]*b, m[1]*b);
        }
        /*! \brief diide each component of this vectors \f$ \vec{a} \f$ and a second number \f$ b \f$
            \param b a number \f$ b \f$
            \return sum \f$ a_i/b \f$
         */
        inline jkVector2 operator/(const double& b) const {
            return jkVector2(m[0]/b, m[1]/b);
        }

        /*! \brief sum assign operator \f$ a_i\leftarrow a_i+b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector2& operator+=(const double& b) {
            m[0]=m[0]+b; m[1]=m[1]+b;
            return *this;
        }
        /*! \brief subtract assign operator \f$ a_i\leftarrow a_i-b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector2& operator-=(const double& b) {
            m[0]=m[0]-b; m[1]=m[1]-b;
            return *this;
        }
        /*! \brief multiply assign operator \f$ a_i\leftarrow a_i\cdot b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector2& operator*=(const double& b) {
            m[0]=m[0]*b; m[1]=m[1]*b;
            return *this;
        }
        /*! \brief divide assign operator \f$ a_i\leftarrow a_i/b \f$
            \param b a number \f$ b \f$
            \return reference to this vector
         */
        inline jkVector2& operator/=(const double& b) {
            m[0]=m[0]/b; m[1]=m[1]/b;
            return *this;
        }

        /** \brief return pointer to data array (2 components of type \c double) */
        double* data() { return m; }

    protected:

        double m[2];
};



/*! \brief dot/scalar product of two vectors \f$ \vec{a}\cdot\vec{b} \f$
    \ingroup tools_math_linalg2

    \param a first input vector
    \param b second input vector
    \return dot/scalar product \f[ \vec{a} \cdot \vec{b}=\sum\limits_{i=1}^2a_ib_i \f]
*/
inline double dotProduct(const jkVector2& a, const jkVector2& b) {
    return a.m[0]*b.m[0]+a.m[1]*b.m[1];
}

/*! \brief multiply a 2D vector with a number (scale the vector)
    \ingroup tools_math_linalg2

    \param a number to multiply with \f$ a \f$
    \param b vector \f$ \vec{b} \f$
    \return \f$ a\cdot\vec{b} \f$
*/
inline jkVector2 operator*(const double& a, const jkVector2& b) {
    return jkVector2(a*b.m[0], a*b.m[1]);
}

/*! \brief rotate a 2D vector \f$ \vec{a} \f$ by a given angle \f$ \alpha \f$ (in radian)

    \f[ \vec{b}=\begin{pmatrix}\cos(\alpha)&-\sin(\alpha)\\\sin(\alpha)&\cos(\alpha) \end{pmatrix}\cdot\vec{a} \f]
*/
inline jkVector2 rotate(jkVector2& in, double angle) {
    const double cosa=cos(angle);
    const double sina=sin(angle);
    jkVector2 out;
    out[0]=cosa*in[0]-sina*in[1];
    out[1]=sina*in[0]+cosa*in[1];
    return out;
}
#endif // LINALG2_H_INCLUDED
