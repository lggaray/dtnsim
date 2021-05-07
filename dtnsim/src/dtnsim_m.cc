//
// Generated file, do not edit! Created by nedtool 5.6 from src/dtnsim.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "dtnsim_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(BundlePkt)

BundlePkt::BundlePkt(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->bundleId = 0;
    this->sourceEid = 0;
    this->destinationEid = 0;
    this->critical = false;
    this->creationTimestamp = 0;
    this->ttl = 0;
    this->returnToSender = false;
    this->custodyTransferRequested = false;
    this->bundleIsCustodyReport = false;
    this->custodyAccepted = false;
    this->custodyBundleId = 0;
    this->custodianEid = 0;
    this->senderEid = 0;
    this->nextHopEid = 0;
    this->hopCount = 0;
    this->xmitCopiesCount = 0;
    this->dlvConfidence = 0;
    this->bundlesCopies = 0;
    this->qos = 0;
}

BundlePkt::BundlePkt(const BundlePkt& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

BundlePkt::~BundlePkt()
{
}

BundlePkt& BundlePkt::operator=(const BundlePkt& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void BundlePkt::copy(const BundlePkt& other)
{
    this->bundleId = other.bundleId;
    this->sourceEid = other.sourceEid;
    this->destinationEid = other.destinationEid;
    this->critical = other.critical;
    this->creationTimestamp = other.creationTimestamp;
    this->ttl = other.ttl;
    this->returnToSender = other.returnToSender;
    this->custodyTransferRequested = other.custodyTransferRequested;
    this->cgrRoute = other.cgrRoute;
    this->bundleIsCustodyReport = other.bundleIsCustodyReport;
    this->custodyAccepted = other.custodyAccepted;
    this->custodyBundleId = other.custodyBundleId;
    this->custodianEid = other.custodianEid;
    this->senderEid = other.senderEid;
    this->nextHopEid = other.nextHopEid;
    this->hopCount = other.hopCount;
    this->visitedNodes = other.visitedNodes;
    this->xmitCopiesCount = other.xmitCopiesCount;
    this->dlvConfidence = other.dlvConfidence;
    this->bundlesCopies = other.bundlesCopies;
    this->qos = other.qos;
}

void BundlePkt::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->bundleId);
    doParsimPacking(b,this->sourceEid);
    doParsimPacking(b,this->destinationEid);
    doParsimPacking(b,this->critical);
    doParsimPacking(b,this->creationTimestamp);
    doParsimPacking(b,this->ttl);
    doParsimPacking(b,this->returnToSender);
    doParsimPacking(b,this->custodyTransferRequested);
    doParsimPacking(b,this->cgrRoute);
    doParsimPacking(b,this->bundleIsCustodyReport);
    doParsimPacking(b,this->custodyAccepted);
    doParsimPacking(b,this->custodyBundleId);
    doParsimPacking(b,this->custodianEid);
    doParsimPacking(b,this->senderEid);
    doParsimPacking(b,this->nextHopEid);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->visitedNodes);
    doParsimPacking(b,this->xmitCopiesCount);
    doParsimPacking(b,this->dlvConfidence);
    doParsimPacking(b,this->bundlesCopies);
    doParsimPacking(b,this->qos);
}

void BundlePkt::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->bundleId);
    doParsimUnpacking(b,this->sourceEid);
    doParsimUnpacking(b,this->destinationEid);
    doParsimUnpacking(b,this->critical);
    doParsimUnpacking(b,this->creationTimestamp);
    doParsimUnpacking(b,this->ttl);
    doParsimUnpacking(b,this->returnToSender);
    doParsimUnpacking(b,this->custodyTransferRequested);
    doParsimUnpacking(b,this->cgrRoute);
    doParsimUnpacking(b,this->bundleIsCustodyReport);
    doParsimUnpacking(b,this->custodyAccepted);
    doParsimUnpacking(b,this->custodyBundleId);
    doParsimUnpacking(b,this->custodianEid);
    doParsimUnpacking(b,this->senderEid);
    doParsimUnpacking(b,this->nextHopEid);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->visitedNodes);
    doParsimUnpacking(b,this->xmitCopiesCount);
    doParsimUnpacking(b,this->dlvConfidence);
    doParsimUnpacking(b,this->bundlesCopies);
    doParsimUnpacking(b,this->qos);
}

long BundlePkt::getBundleId() const
{
    return this->bundleId;
}

void BundlePkt::setBundleId(long bundleId)
{
    this->bundleId = bundleId;
}

int BundlePkt::getSourceEid() const
{
    return this->sourceEid;
}

void BundlePkt::setSourceEid(int sourceEid)
{
    this->sourceEid = sourceEid;
}

int BundlePkt::getDestinationEid() const
{
    return this->destinationEid;
}

void BundlePkt::setDestinationEid(int destinationEid)
{
    this->destinationEid = destinationEid;
}

bool BundlePkt::getCritical() const
{
    return this->critical;
}

void BundlePkt::setCritical(bool critical)
{
    this->critical = critical;
}

::omnetpp::simtime_t BundlePkt::getCreationTimestamp() const
{
    return this->creationTimestamp;
}

void BundlePkt::setCreationTimestamp(::omnetpp::simtime_t creationTimestamp)
{
    this->creationTimestamp = creationTimestamp;
}

::omnetpp::simtime_t BundlePkt::getTtl() const
{
    return this->ttl;
}

void BundlePkt::setTtl(::omnetpp::simtime_t ttl)
{
    this->ttl = ttl;
}

bool BundlePkt::getReturnToSender() const
{
    return this->returnToSender;
}

void BundlePkt::setReturnToSender(bool returnToSender)
{
    this->returnToSender = returnToSender;
}

bool BundlePkt::getCustodyTransferRequested() const
{
    return this->custodyTransferRequested;
}

void BundlePkt::setCustodyTransferRequested(bool custodyTransferRequested)
{
    this->custodyTransferRequested = custodyTransferRequested;
}

CgrRoute& BundlePkt::getCgrRoute()
{
    return this->cgrRoute;
}

void BundlePkt::setCgrRoute(const CgrRoute& cgrRoute)
{
    this->cgrRoute = cgrRoute;
}

bool BundlePkt::getBundleIsCustodyReport() const
{
    return this->bundleIsCustodyReport;
}

void BundlePkt::setBundleIsCustodyReport(bool bundleIsCustodyReport)
{
    this->bundleIsCustodyReport = bundleIsCustodyReport;
}

bool BundlePkt::getCustodyAccepted() const
{
    return this->custodyAccepted;
}

void BundlePkt::setCustodyAccepted(bool custodyAccepted)
{
    this->custodyAccepted = custodyAccepted;
}

long BundlePkt::getCustodyBundleId() const
{
    return this->custodyBundleId;
}

void BundlePkt::setCustodyBundleId(long custodyBundleId)
{
    this->custodyBundleId = custodyBundleId;
}

int BundlePkt::getCustodianEid() const
{
    return this->custodianEid;
}

void BundlePkt::setCustodianEid(int custodianEid)
{
    this->custodianEid = custodianEid;
}

int BundlePkt::getSenderEid() const
{
    return this->senderEid;
}

void BundlePkt::setSenderEid(int senderEid)
{
    this->senderEid = senderEid;
}

int BundlePkt::getNextHopEid() const
{
    return this->nextHopEid;
}

void BundlePkt::setNextHopEid(int nextHopEid)
{
    this->nextHopEid = nextHopEid;
}

int BundlePkt::getHopCount() const
{
    return this->hopCount;
}

void BundlePkt::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

intList& BundlePkt::getVisitedNodes()
{
    return this->visitedNodes;
}

void BundlePkt::setVisitedNodes(const intList& visitedNodes)
{
    this->visitedNodes = visitedNodes;
}

int BundlePkt::getXmitCopiesCount() const
{
    return this->xmitCopiesCount;
}

void BundlePkt::setXmitCopiesCount(int xmitCopiesCount)
{
    this->xmitCopiesCount = xmitCopiesCount;
}

double BundlePkt::getDlvConfidence() const
{
    return this->dlvConfidence;
}

void BundlePkt::setDlvConfidence(double dlvConfidence)
{
    this->dlvConfidence = dlvConfidence;
}

int BundlePkt::getBundlesCopies() const
{
    return this->bundlesCopies;
}

void BundlePkt::setBundlesCopies(int bundlesCopies)
{
    this->bundlesCopies = bundlesCopies;
}

int BundlePkt::getQos() const
{
    return this->qos;
}

void BundlePkt::setQos(int qos)
{
    this->qos = qos;
}

class BundlePktDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    BundlePktDescriptor();
    virtual ~BundlePktDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(BundlePktDescriptor)

BundlePktDescriptor::BundlePktDescriptor() : omnetpp::cClassDescriptor("BundlePkt", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

BundlePktDescriptor::~BundlePktDescriptor()
{
    delete[] propertynames;
}

bool BundlePktDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BundlePkt *>(obj)!=nullptr;
}

const char **BundlePktDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *BundlePktDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int BundlePktDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 21+basedesc->getFieldCount() : 21;
}

unsigned int BundlePktDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<21) ? fieldTypeFlags[field] : 0;
}

const char *BundlePktDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "bundleId",
        "sourceEid",
        "destinationEid",
        "critical",
        "creationTimestamp",
        "ttl",
        "returnToSender",
        "custodyTransferRequested",
        "cgrRoute",
        "bundleIsCustodyReport",
        "custodyAccepted",
        "custodyBundleId",
        "custodianEid",
        "senderEid",
        "nextHopEid",
        "hopCount",
        "visitedNodes",
        "xmitCopiesCount",
        "dlvConfidence",
        "bundlesCopies",
        "qos",
    };
    return (field>=0 && field<21) ? fieldNames[field] : nullptr;
}

int BundlePktDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundleId")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceEid")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationEid")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "critical")==0) return base+3;
    if (fieldName[0]=='c' && strcmp(fieldName, "creationTimestamp")==0) return base+4;
    if (fieldName[0]=='t' && strcmp(fieldName, "ttl")==0) return base+5;
    if (fieldName[0]=='r' && strcmp(fieldName, "returnToSender")==0) return base+6;
    if (fieldName[0]=='c' && strcmp(fieldName, "custodyTransferRequested")==0) return base+7;
    if (fieldName[0]=='c' && strcmp(fieldName, "cgrRoute")==0) return base+8;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundleIsCustodyReport")==0) return base+9;
    if (fieldName[0]=='c' && strcmp(fieldName, "custodyAccepted")==0) return base+10;
    if (fieldName[0]=='c' && strcmp(fieldName, "custodyBundleId")==0) return base+11;
    if (fieldName[0]=='c' && strcmp(fieldName, "custodianEid")==0) return base+12;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderEid")==0) return base+13;
    if (fieldName[0]=='n' && strcmp(fieldName, "nextHopEid")==0) return base+14;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+15;
    if (fieldName[0]=='v' && strcmp(fieldName, "visitedNodes")==0) return base+16;
    if (fieldName[0]=='x' && strcmp(fieldName, "xmitCopiesCount")==0) return base+17;
    if (fieldName[0]=='d' && strcmp(fieldName, "dlvConfidence")==0) return base+18;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundlesCopies")==0) return base+19;
    if (fieldName[0]=='q' && strcmp(fieldName, "qos")==0) return base+20;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *BundlePktDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "long",
        "int",
        "int",
        "bool",
        "simtime_t",
        "simtime_t",
        "bool",
        "bool",
        "CgrRoute",
        "bool",
        "bool",
        "long",
        "int",
        "int",
        "int",
        "int",
        "intList",
        "int",
        "double",
        "int",
        "int",
    };
    return (field>=0 && field<21) ? fieldTypeStrings[field] : nullptr;
}

const char **BundlePktDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BundlePktDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BundlePktDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    BundlePkt *pp = (BundlePkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *BundlePktDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BundlePkt *pp = (BundlePkt *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string BundlePktDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BundlePkt *pp = (BundlePkt *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getBundleId());
        case 1: return long2string(pp->getSourceEid());
        case 2: return long2string(pp->getDestinationEid());
        case 3: return bool2string(pp->getCritical());
        case 4: return simtime2string(pp->getCreationTimestamp());
        case 5: return simtime2string(pp->getTtl());
        case 6: return bool2string(pp->getReturnToSender());
        case 7: return bool2string(pp->getCustodyTransferRequested());
        case 8: {std::stringstream out; out << pp->getCgrRoute(); return out.str();}
        case 9: return bool2string(pp->getBundleIsCustodyReport());
        case 10: return bool2string(pp->getCustodyAccepted());
        case 11: return long2string(pp->getCustodyBundleId());
        case 12: return long2string(pp->getCustodianEid());
        case 13: return long2string(pp->getSenderEid());
        case 14: return long2string(pp->getNextHopEid());
        case 15: return long2string(pp->getHopCount());
        case 16: {std::stringstream out; out << pp->getVisitedNodes(); return out.str();}
        case 17: return long2string(pp->getXmitCopiesCount());
        case 18: return double2string(pp->getDlvConfidence());
        case 19: return long2string(pp->getBundlesCopies());
        case 20: return long2string(pp->getQos());
        default: return "";
    }
}

bool BundlePktDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    BundlePkt *pp = (BundlePkt *)object; (void)pp;
    switch (field) {
        case 0: pp->setBundleId(string2long(value)); return true;
        case 1: pp->setSourceEid(string2long(value)); return true;
        case 2: pp->setDestinationEid(string2long(value)); return true;
        case 3: pp->setCritical(string2bool(value)); return true;
        case 4: pp->setCreationTimestamp(string2simtime(value)); return true;
        case 5: pp->setTtl(string2simtime(value)); return true;
        case 6: pp->setReturnToSender(string2bool(value)); return true;
        case 7: pp->setCustodyTransferRequested(string2bool(value)); return true;
        case 9: pp->setBundleIsCustodyReport(string2bool(value)); return true;
        case 10: pp->setCustodyAccepted(string2bool(value)); return true;
        case 11: pp->setCustodyBundleId(string2long(value)); return true;
        case 12: pp->setCustodianEid(string2long(value)); return true;
        case 13: pp->setSenderEid(string2long(value)); return true;
        case 14: pp->setNextHopEid(string2long(value)); return true;
        case 15: pp->setHopCount(string2long(value)); return true;
        case 17: pp->setXmitCopiesCount(string2long(value)); return true;
        case 18: pp->setDlvConfidence(string2double(value)); return true;
        case 19: pp->setBundlesCopies(string2long(value)); return true;
        case 20: pp->setQos(string2long(value)); return true;
        default: return false;
    }
}

const char *BundlePktDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 8: return omnetpp::opp_typename(typeid(CgrRoute));
        case 16: return omnetpp::opp_typename(typeid(intList));
        default: return nullptr;
    };
}

void *BundlePktDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    BundlePkt *pp = (BundlePkt *)object; (void)pp;
    switch (field) {
        case 8: return (void *)(&pp->getCgrRoute()); break;
        case 16: return (void *)(&pp->getVisitedNodes()); break;
        default: return nullptr;
    }
}

Register_Class(TrafficGeneratorMsg)

TrafficGeneratorMsg::TrafficGeneratorMsg(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->bundlesNumber = 0;
    this->destinationEid = 0;
    this->size = 0;
    this->ttl = 0;
    this->interval = 0;
}

TrafficGeneratorMsg::TrafficGeneratorMsg(const TrafficGeneratorMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

TrafficGeneratorMsg::~TrafficGeneratorMsg()
{
}

TrafficGeneratorMsg& TrafficGeneratorMsg::operator=(const TrafficGeneratorMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void TrafficGeneratorMsg::copy(const TrafficGeneratorMsg& other)
{
    this->bundlesNumber = other.bundlesNumber;
    this->destinationEid = other.destinationEid;
    this->size = other.size;
    this->ttl = other.ttl;
    this->interval = other.interval;
}

void TrafficGeneratorMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->bundlesNumber);
    doParsimPacking(b,this->destinationEid);
    doParsimPacking(b,this->size);
    doParsimPacking(b,this->ttl);
    doParsimPacking(b,this->interval);
}

void TrafficGeneratorMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->bundlesNumber);
    doParsimUnpacking(b,this->destinationEid);
    doParsimUnpacking(b,this->size);
    doParsimUnpacking(b,this->ttl);
    doParsimUnpacking(b,this->interval);
}

int TrafficGeneratorMsg::getBundlesNumber() const
{
    return this->bundlesNumber;
}

void TrafficGeneratorMsg::setBundlesNumber(int bundlesNumber)
{
    this->bundlesNumber = bundlesNumber;
}

int TrafficGeneratorMsg::getDestinationEid() const
{
    return this->destinationEid;
}

void TrafficGeneratorMsg::setDestinationEid(int destinationEid)
{
    this->destinationEid = destinationEid;
}

int TrafficGeneratorMsg::getSize() const
{
    return this->size;
}

void TrafficGeneratorMsg::setSize(int size)
{
    this->size = size;
}

double TrafficGeneratorMsg::getTtl() const
{
    return this->ttl;
}

void TrafficGeneratorMsg::setTtl(double ttl)
{
    this->ttl = ttl;
}

double TrafficGeneratorMsg::getInterval() const
{
    return this->interval;
}

void TrafficGeneratorMsg::setInterval(double interval)
{
    this->interval = interval;
}

class TrafficGeneratorMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TrafficGeneratorMsgDescriptor();
    virtual ~TrafficGeneratorMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(TrafficGeneratorMsgDescriptor)

TrafficGeneratorMsgDescriptor::TrafficGeneratorMsgDescriptor() : omnetpp::cClassDescriptor("TrafficGeneratorMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

TrafficGeneratorMsgDescriptor::~TrafficGeneratorMsgDescriptor()
{
    delete[] propertynames;
}

bool TrafficGeneratorMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TrafficGeneratorMsg *>(obj)!=nullptr;
}

const char **TrafficGeneratorMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TrafficGeneratorMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TrafficGeneratorMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount() : 5;
}

unsigned int TrafficGeneratorMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *TrafficGeneratorMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "bundlesNumber",
        "destinationEid",
        "size",
        "ttl",
        "interval",
    };
    return (field>=0 && field<5) ? fieldNames[field] : nullptr;
}

int TrafficGeneratorMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundlesNumber")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationEid")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "size")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "ttl")==0) return base+3;
    if (fieldName[0]=='i' && strcmp(fieldName, "interval")==0) return base+4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TrafficGeneratorMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "double",
        "double",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : nullptr;
}

const char **TrafficGeneratorMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TrafficGeneratorMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TrafficGeneratorMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *TrafficGeneratorMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TrafficGeneratorMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getBundlesNumber());
        case 1: return long2string(pp->getDestinationEid());
        case 2: return long2string(pp->getSize());
        case 3: return double2string(pp->getTtl());
        case 4: return double2string(pp->getInterval());
        default: return "";
    }
}

bool TrafficGeneratorMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setBundlesNumber(string2long(value)); return true;
        case 1: pp->setDestinationEid(string2long(value)); return true;
        case 2: pp->setSize(string2long(value)); return true;
        case 3: pp->setTtl(string2double(value)); return true;
        case 4: pp->setInterval(string2double(value)); return true;
        default: return false;
    }
}

const char *TrafficGeneratorMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *TrafficGeneratorMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TrafficGeneratorMsg *pp = (TrafficGeneratorMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(ContactMsg)

ContactMsg::ContactMsg(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->id = 0;
    this->dataRate = 0;
    this->start = 0;
    this->end = 0;
    this->duration = 0;
    this->sourceEid = 0;
    this->destinationEid = 0;
}

ContactMsg::ContactMsg(const ContactMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ContactMsg::~ContactMsg()
{
}

ContactMsg& ContactMsg::operator=(const ContactMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ContactMsg::copy(const ContactMsg& other)
{
    this->id = other.id;
    this->dataRate = other.dataRate;
    this->start = other.start;
    this->end = other.end;
    this->duration = other.duration;
    this->sourceEid = other.sourceEid;
    this->destinationEid = other.destinationEid;
}

void ContactMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->id);
    doParsimPacking(b,this->dataRate);
    doParsimPacking(b,this->start);
    doParsimPacking(b,this->end);
    doParsimPacking(b,this->duration);
    doParsimPacking(b,this->sourceEid);
    doParsimPacking(b,this->destinationEid);
}

void ContactMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->id);
    doParsimUnpacking(b,this->dataRate);
    doParsimUnpacking(b,this->start);
    doParsimUnpacking(b,this->end);
    doParsimUnpacking(b,this->duration);
    doParsimUnpacking(b,this->sourceEid);
    doParsimUnpacking(b,this->destinationEid);
}

int ContactMsg::getId() const
{
    return this->id;
}

void ContactMsg::setId(int id)
{
    this->id = id;
}

double ContactMsg::getDataRate() const
{
    return this->dataRate;
}

void ContactMsg::setDataRate(double dataRate)
{
    this->dataRate = dataRate;
}

::omnetpp::simtime_t ContactMsg::getStart() const
{
    return this->start;
}

void ContactMsg::setStart(::omnetpp::simtime_t start)
{
    this->start = start;
}

::omnetpp::simtime_t ContactMsg::getEnd() const
{
    return this->end;
}

void ContactMsg::setEnd(::omnetpp::simtime_t end)
{
    this->end = end;
}

::omnetpp::simtime_t ContactMsg::getDuration() const
{
    return this->duration;
}

void ContactMsg::setDuration(::omnetpp::simtime_t duration)
{
    this->duration = duration;
}

int ContactMsg::getSourceEid() const
{
    return this->sourceEid;
}

void ContactMsg::setSourceEid(int sourceEid)
{
    this->sourceEid = sourceEid;
}

int ContactMsg::getDestinationEid() const
{
    return this->destinationEid;
}

void ContactMsg::setDestinationEid(int destinationEid)
{
    this->destinationEid = destinationEid;
}

class ContactMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ContactMsgDescriptor();
    virtual ~ContactMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ContactMsgDescriptor)

ContactMsgDescriptor::ContactMsgDescriptor() : omnetpp::cClassDescriptor("ContactMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

ContactMsgDescriptor::~ContactMsgDescriptor()
{
    delete[] propertynames;
}

bool ContactMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ContactMsg *>(obj)!=nullptr;
}

const char **ContactMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ContactMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ContactMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount() : 7;
}

unsigned int ContactMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *ContactMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "id",
        "dataRate",
        "start",
        "end",
        "duration",
        "sourceEid",
        "destinationEid",
    };
    return (field>=0 && field<7) ? fieldNames[field] : nullptr;
}

int ContactMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "id")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "dataRate")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "start")==0) return base+2;
    if (fieldName[0]=='e' && strcmp(fieldName, "end")==0) return base+3;
    if (fieldName[0]=='d' && strcmp(fieldName, "duration")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceEid")==0) return base+5;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationEid")==0) return base+6;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ContactMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "simtime_t",
        "simtime_t",
        "simtime_t",
        "int",
        "int",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : nullptr;
}

const char **ContactMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ContactMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ContactMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ContactMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ContactMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getId());
        case 1: return double2string(pp->getDataRate());
        case 2: return simtime2string(pp->getStart());
        case 3: return simtime2string(pp->getEnd());
        case 4: return simtime2string(pp->getDuration());
        case 5: return long2string(pp->getSourceEid());
        case 6: return long2string(pp->getDestinationEid());
        default: return "";
    }
}

bool ContactMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setId(string2long(value)); return true;
        case 1: pp->setDataRate(string2double(value)); return true;
        case 2: pp->setStart(string2simtime(value)); return true;
        case 3: pp->setEnd(string2simtime(value)); return true;
        case 4: pp->setDuration(string2simtime(value)); return true;
        case 5: pp->setSourceEid(string2long(value)); return true;
        case 6: pp->setDestinationEid(string2long(value)); return true;
        default: return false;
    }
}

const char *ContactMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *ContactMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ContactMsg *pp = (ContactMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(ForwardingMsgEnd)

ForwardingMsgEnd::ForwardingMsgEnd(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->neighborEid = 0;
    this->contactId = 0;
    this->bundleId = 0;
    this->sentToDestination = false;
}

ForwardingMsgEnd::ForwardingMsgEnd(const ForwardingMsgEnd& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ForwardingMsgEnd::~ForwardingMsgEnd()
{
}

ForwardingMsgEnd& ForwardingMsgEnd::operator=(const ForwardingMsgEnd& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ForwardingMsgEnd::copy(const ForwardingMsgEnd& other)
{
    this->neighborEid = other.neighborEid;
    this->contactId = other.contactId;
    this->bundleId = other.bundleId;
    this->sentToDestination = other.sentToDestination;
}

void ForwardingMsgEnd::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->neighborEid);
    doParsimPacking(b,this->contactId);
    doParsimPacking(b,this->bundleId);
    doParsimPacking(b,this->sentToDestination);
}

void ForwardingMsgEnd::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->neighborEid);
    doParsimUnpacking(b,this->contactId);
    doParsimUnpacking(b,this->bundleId);
    doParsimUnpacking(b,this->sentToDestination);
}

int ForwardingMsgEnd::getNeighborEid() const
{
    return this->neighborEid;
}

void ForwardingMsgEnd::setNeighborEid(int neighborEid)
{
    this->neighborEid = neighborEid;
}

int ForwardingMsgEnd::getContactId() const
{
    return this->contactId;
}

void ForwardingMsgEnd::setContactId(int contactId)
{
    this->contactId = contactId;
}

long ForwardingMsgEnd::getBundleId() const
{
    return this->bundleId;
}

void ForwardingMsgEnd::setBundleId(long bundleId)
{
    this->bundleId = bundleId;
}

bool ForwardingMsgEnd::getSentToDestination() const
{
    return this->sentToDestination;
}

void ForwardingMsgEnd::setSentToDestination(bool sentToDestination)
{
    this->sentToDestination = sentToDestination;
}

class ForwardingMsgEndDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ForwardingMsgEndDescriptor();
    virtual ~ForwardingMsgEndDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ForwardingMsgEndDescriptor)

ForwardingMsgEndDescriptor::ForwardingMsgEndDescriptor() : omnetpp::cClassDescriptor("ForwardingMsgEnd", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

ForwardingMsgEndDescriptor::~ForwardingMsgEndDescriptor()
{
    delete[] propertynames;
}

bool ForwardingMsgEndDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ForwardingMsgEnd *>(obj)!=nullptr;
}

const char **ForwardingMsgEndDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ForwardingMsgEndDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ForwardingMsgEndDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount() : 4;
}

unsigned int ForwardingMsgEndDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *ForwardingMsgEndDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "neighborEid",
        "contactId",
        "bundleId",
        "sentToDestination",
    };
    return (field>=0 && field<4) ? fieldNames[field] : nullptr;
}

int ForwardingMsgEndDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "neighborEid")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "contactId")==0) return base+1;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundleId")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "sentToDestination")==0) return base+3;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ForwardingMsgEndDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "long",
        "bool",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : nullptr;
}

const char **ForwardingMsgEndDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ForwardingMsgEndDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ForwardingMsgEndDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgEnd *pp = (ForwardingMsgEnd *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ForwardingMsgEndDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgEnd *pp = (ForwardingMsgEnd *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ForwardingMsgEndDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgEnd *pp = (ForwardingMsgEnd *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getNeighborEid());
        case 1: return long2string(pp->getContactId());
        case 2: return long2string(pp->getBundleId());
        case 3: return bool2string(pp->getSentToDestination());
        default: return "";
    }
}

bool ForwardingMsgEndDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgEnd *pp = (ForwardingMsgEnd *)object; (void)pp;
    switch (field) {
        case 0: pp->setNeighborEid(string2long(value)); return true;
        case 1: pp->setContactId(string2long(value)); return true;
        case 2: pp->setBundleId(string2long(value)); return true;
        case 3: pp->setSentToDestination(string2bool(value)); return true;
        default: return false;
    }
}

const char *ForwardingMsgEndDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *ForwardingMsgEndDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgEnd *pp = (ForwardingMsgEnd *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(ForwardingMsgStart)

ForwardingMsgStart::ForwardingMsgStart(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->neighborEid = 0;
    this->contactId = 0;
}

ForwardingMsgStart::ForwardingMsgStart(const ForwardingMsgStart& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ForwardingMsgStart::~ForwardingMsgStart()
{
}

ForwardingMsgStart& ForwardingMsgStart::operator=(const ForwardingMsgStart& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ForwardingMsgStart::copy(const ForwardingMsgStart& other)
{
    this->neighborEid = other.neighborEid;
    this->contactId = other.contactId;
}

void ForwardingMsgStart::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->neighborEid);
    doParsimPacking(b,this->contactId);
}

void ForwardingMsgStart::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->neighborEid);
    doParsimUnpacking(b,this->contactId);
}

int ForwardingMsgStart::getNeighborEid() const
{
    return this->neighborEid;
}

void ForwardingMsgStart::setNeighborEid(int neighborEid)
{
    this->neighborEid = neighborEid;
}

int ForwardingMsgStart::getContactId() const
{
    return this->contactId;
}

void ForwardingMsgStart::setContactId(int contactId)
{
    this->contactId = contactId;
}

class ForwardingMsgStartDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ForwardingMsgStartDescriptor();
    virtual ~ForwardingMsgStartDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ForwardingMsgStartDescriptor)

ForwardingMsgStartDescriptor::ForwardingMsgStartDescriptor() : omnetpp::cClassDescriptor("ForwardingMsgStart", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

ForwardingMsgStartDescriptor::~ForwardingMsgStartDescriptor()
{
    delete[] propertynames;
}

bool ForwardingMsgStartDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ForwardingMsgStart *>(obj)!=nullptr;
}

const char **ForwardingMsgStartDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ForwardingMsgStartDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ForwardingMsgStartDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int ForwardingMsgStartDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ForwardingMsgStartDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "neighborEid",
        "contactId",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int ForwardingMsgStartDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "neighborEid")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "contactId")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ForwardingMsgStartDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **ForwardingMsgStartDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ForwardingMsgStartDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ForwardingMsgStartDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgStart *pp = (ForwardingMsgStart *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *ForwardingMsgStartDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgStart *pp = (ForwardingMsgStart *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ForwardingMsgStartDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgStart *pp = (ForwardingMsgStart *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getNeighborEid());
        case 1: return long2string(pp->getContactId());
        default: return "";
    }
}

bool ForwardingMsgStartDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgStart *pp = (ForwardingMsgStart *)object; (void)pp;
    switch (field) {
        case 0: pp->setNeighborEid(string2long(value)); return true;
        case 1: pp->setContactId(string2long(value)); return true;
        default: return false;
    }
}

const char *ForwardingMsgStartDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *ForwardingMsgStartDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ForwardingMsgStart *pp = (ForwardingMsgStart *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(CustodyTimout)

CustodyTimout::CustodyTimout(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->bundleId = 0;
}

CustodyTimout::CustodyTimout(const CustodyTimout& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

CustodyTimout::~CustodyTimout()
{
}

CustodyTimout& CustodyTimout::operator=(const CustodyTimout& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void CustodyTimout::copy(const CustodyTimout& other)
{
    this->bundleId = other.bundleId;
}

void CustodyTimout::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->bundleId);
}

void CustodyTimout::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->bundleId);
}

int CustodyTimout::getBundleId() const
{
    return this->bundleId;
}

void CustodyTimout::setBundleId(int bundleId)
{
    this->bundleId = bundleId;
}

class CustodyTimoutDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    CustodyTimoutDescriptor();
    virtual ~CustodyTimoutDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(CustodyTimoutDescriptor)

CustodyTimoutDescriptor::CustodyTimoutDescriptor() : omnetpp::cClassDescriptor("CustodyTimout", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

CustodyTimoutDescriptor::~CustodyTimoutDescriptor()
{
    delete[] propertynames;
}

bool CustodyTimoutDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CustodyTimout *>(obj)!=nullptr;
}

const char **CustodyTimoutDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *CustodyTimoutDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int CustodyTimoutDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int CustodyTimoutDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *CustodyTimoutDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "bundleId",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int CustodyTimoutDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bundleId")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *CustodyTimoutDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **CustodyTimoutDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CustodyTimoutDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CustodyTimoutDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    CustodyTimout *pp = (CustodyTimout *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *CustodyTimoutDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    CustodyTimout *pp = (CustodyTimout *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CustodyTimoutDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    CustodyTimout *pp = (CustodyTimout *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getBundleId());
        default: return "";
    }
}

bool CustodyTimoutDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    CustodyTimout *pp = (CustodyTimout *)object; (void)pp;
    switch (field) {
        case 0: pp->setBundleId(string2long(value)); return true;
        default: return false;
    }
}

const char *CustodyTimoutDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *CustodyTimoutDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    CustodyTimout *pp = (CustodyTimout *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


