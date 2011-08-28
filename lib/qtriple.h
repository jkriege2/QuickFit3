#ifndef QTRIPLE_H
#define QTRIPLE_H

#include <QtCore/qdatastream.h>

template <class T1, class T2, class T3>
struct QTriple
{
    typedef T1 first_type;
    typedef T2 second_type;
    typedef T3 third_type;

    QTriple() : first(T1()), second(T2()), third(T3()) {}
    QTriple(const T1 &t1, const T2 &t2, const T3 &t3) : first(t1), second(t2), third(t3) {}

    QTriple<T1, T2, T3> &operator=(const QTriple<T1, T2, T3> &other)
    { first = other.first; second = other.second; third=other.third; return *this; }

    T1 first;
    T2 second;
    T3 third;
};

template <class T1, class T2, class T3>
inline bool operator==(const QTriple<T1, T2, T3> &p1, const QTriple<T1, T2, T3> &p2)
{ return p1.first == p2.first && p1.second == p2.second && p1.third == p2.third; }

template <class T1, class T2, class T3>
inline bool operator!=(const QTriple<T1, T2, T3> &p1, const QTriple<T1, T2, T3> &p2)
{ return !(p1 == p2); }

template <class T1, class T2, class T3>
inline bool operator<(const QTriple<T1, T2, T3> &p1, const QTriple<T1, T2, T3> &p2)
{
    return p1.first < p2.first || (!(p2.first < p1.first) && p1.second < p2.second) || (!(p2.first < p1.first) && !(p1.second < p2.second) && (p1.third < p2.third));
}

template <class T1, class T2, class T3>
inline bool operator>(const QTriple<T1, T2, T3> &p1, const QTriple<T1, T2, T3> &p2)
{
    return p2 < p1;
}

template <class T1, class T2, class T3>
inline bool operator<=(const QTriple<T1, T2, T3> &p1, const QTriple<T1, T2, T3> &p2)
{
    return !(p2 < p1);
}

template <class T1, class T2, class T3>
inline bool operator>=(const QTriple<T1, T2, T3> &p1, const QTriple<T1, T2, T3> &p2)
{
    return !(p1 < p2);
}

template <class T1, class T2, class T3>
QTriple<T1, T2, T3> qMakeTriple(const T1 &x, const T2 &y, const T3 &z)
{
    return QTriple<T1, T2, T3>(x, y, z);
}

#ifndef QT_NO_DATASTREAM
template <class T1, class T2, class T3>
inline QDataStream& operator>>(QDataStream& s, QTriple<T1, T2, T3>& p)
{
    s >> p.first >> p.second >> p.third;
    return s;
}

template <class T1, class T2, class T3>
inline QDataStream& operator<<(QDataStream& s, const QTriple<T1, T2, T3>& p)
{
    s << p.first << p.second << p.third;
    return s;
}
#endif

#endif // QTRIPLE_H
