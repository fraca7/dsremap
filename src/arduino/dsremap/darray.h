
#ifndef _DARRAY_H
#define _DARRAY_H

template <typename T> class DArray
{
public:
  DArray()
    : m_Items((T**)malloc(sizeof(T*))),
      m_Count(0),
      m_Cap(1)
  {
  }

  ~DArray()
  {
    Clear();
    free(m_Items);
  }

  void AddItem(T* item)
  {
    if (m_Count == m_Cap) {
      m_Cap *= 2;
      m_Items = (T**)realloc((void*)m_Items, m_Cap * sizeof(T*));
    }
    m_Items[m_Count++] = item;
  }

  void Clear()
  {
    for (uint8_t i = 0; i < m_Count; ++i)
      delete m_Items[i];
    m_Count = 0;
  }

  uint8_t Count() const {
    return m_Count;
  }

  T* GetItem(uint8_t idx) const {
    return m_Items[idx];
  }

private:
  T** m_Items;
  uint8_t m_Count;
  uint8_t m_Cap;
};

#endif /* _DARRAY_H */
