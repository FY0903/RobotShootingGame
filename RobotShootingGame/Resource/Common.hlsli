cbuffer World : register(b0)
{
    Matrix World : packoffset(c0);
}

cbuffer VP : register(b1)
{
    Matrix View : packoffset(c0);
    Matrix Projection : packoffset(c4);
}
