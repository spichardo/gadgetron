#ifndef proteus_export_H_
#define proteus_export_H_


#if defined (WIN32)
#if defined (PROTEUS_EXPORTS)
#define EXPORTGADGETSPROTEUS __declspec(dllexport)
#else
#define EXPORTGADGETSPROTEUS __declspec(dllimport)
#endif
#else
#define EXPORTGADGETSPROTEUS
#endif

#endif /* proteus_export_H_ */
