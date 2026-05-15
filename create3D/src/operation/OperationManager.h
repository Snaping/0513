#pragma once

#include "Types.h"
#include "SketchManager.h"
#include "Mesh.h"

namespace Create3D {

enum class OperationType
{
    None,
    Extrude,
    Revolve,
    Sweep,
    Loft,
    Cut
};

class Operation;
using OperationPtr = SharedPtr<Operation>;

class Operation
{
public:
    virtual ~Operation() = default;
    virtual OperationType GetType() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual SharedPtr<Mesh> Execute() = 0;
    virtual void UpdatePreview() = 0;
    virtual bool IsValid() const = 0;

    void SetActive(bool active) { m_Active = active; }
    bool IsActive() const { return m_Active; }

protected:
    bool m_Active = false;
};

struct ExtrudeParams
{
    SharedPtr<Sketch> sketch;
    f32 depth = 1.0f;
    Vec3 direction = Vec3(0, 0, 1);
    bool symmetric = false;
    f32 taperAngle = 0.0f;
    int segments = 1;
};

class ExtrudeOperation : public Operation
{
public:
    ExtrudeOperation();

    OperationType GetType() const override { return OperationType::Extrude; }
    const std::string& GetName() const override { static std::string name = "Extrude"; return name; }

    void SetParams(const ExtrudeParams& params) { m_Params = params; }
    const ExtrudeParams& GetParams() const { return m_Params; }
    ExtrudeParams& GetParams() { return m_Params; }

    SharedPtr<Mesh> Execute() override;
    void UpdatePreview() override;
    bool IsValid() const override;

    static SharedPtr<Mesh> ExtrudeSketch(
        SharedPtr<Sketch> sketch,
        f32 depth,
        const Vec3& direction,
        f32 taperAngle = 0.0f,
        int segments = 1
    );

private:
    ExtrudeParams m_Params;
};

struct RevolveParams
{
    SharedPtr<Sketch> sketch;
    Vec3 axisOrigin = Vec3(0, 0, 0);
    Vec3 axisDirection = Vec3(0, 1, 0);
    f32 angle = 360.0f;
    int segments = 32;
};

class RevolveOperation : public Operation
{
public:
    RevolveOperation();

    OperationType GetType() const override { return OperationType::Revolve; }
    const std::string& GetName() const override { static std::string name = "Revolve"; return name; }

    void SetParams(const RevolveParams& params) { m_Params = params; }
    const RevolveParams& GetParams() const { return m_Params; }
    RevolveParams& GetParams() { return m_Params; }

    SharedPtr<Mesh> Execute() override;
    void UpdatePreview() override;
    bool IsValid() const override;

    static SharedPtr<Mesh> RevolveSketch(
        SharedPtr<Sketch> sketch,
        const Vec3& axisOrigin,
        const Vec3& axisDirection,
        f32 angle,
        int segments
    );

private:
    RevolveParams m_Params;
};

struct SweepParams
{
    SharedPtr<Sketch> profile;
    SharedPtr<Sketch> path;
    bool maintainOrientation = true;
    int segments = 20;
};

class SweepOperation : public Operation
{
public:
    SweepOperation();

    OperationType GetType() const override { return OperationType::Sweep; }
    const std::string& GetName() const override { static std::string name = "Sweep"; return name; }

    void SetParams(const SweepParams& params) { m_Params = params; }
    const SweepParams& GetParams() const { return m_Params; }
    SweepParams& GetParams() { return m_Params; }

    SharedPtr<Mesh> Execute() override;
    void UpdatePreview() override;
    bool IsValid() const override;

    static SharedPtr<Mesh> SweepSketch(
        SharedPtr<Sketch> profile,
        SharedPtr<Sketch> path,
        int segments,
        bool maintainOrientation
    );

private:
    SweepParams m_Params;
};

struct LoftParams
{
    std::vector<SharedPtr<Sketch>> sections;
    bool closed = false;
    int segments = 20;
};

class LoftOperation : public Operation
{
public:
    LoftOperation();

    OperationType GetType() const override { return OperationType::Loft; }
    const std::string& GetName() const override { static std::string name = "Loft"; return name; }

    void SetParams(const LoftParams& params) { m_Params = params; }
    const LoftParams& GetParams() const { return m_Params; }
    LoftParams& GetParams() { return m_Params; }

    SharedPtr<Mesh> Execute() override;
    void UpdatePreview() override;
    bool IsValid() const override;

    static SharedPtr<Mesh> LoftSketches(
        const std::vector<SharedPtr<Sketch>>& sections,
        int segments,
        bool closed
    );

private:
    LoftParams m_Params;
};

class OperationManager
{
public:
    OperationManager();
    ~OperationManager();

    bool Initialize();
    void Shutdown();
    void Update(f64 deltaTime);

    void SetCurrentOperation(OperationType type);
    OperationType GetCurrentOperationType() const { return m_CurrentOperationType; }

    SharedPtr<Operation> GetCurrentOperation() const { return m_CurrentOperation; }

    SharedPtr<ExtrudeOperation> GetExtrudeOperation() const;
    SharedPtr<RevolveOperation> GetRevolveOperation() const;
    SharedPtr<SweepOperation> GetSweepOperation() const;
    SharedPtr<LoftOperation> GetLoftOperation() const;

    void ExecuteCurrentOperation();
    void CancelCurrentOperation();

    SharedPtr<Mesh> Extrude(const ExtrudeParams& params);
    SharedPtr<Mesh> Revolve(const RevolveParams& params);
    SharedPtr<Mesh> Sweep(const SweepParams& params);
    SharedPtr<Mesh> Loft(const LoftParams& params);

    SharedPtr<Mesh> GetPreviewMesh() const { return m_PreviewMesh; }
    bool HasPreview() const { return m_PreviewMesh != nullptr; }

private:
    OperationType m_CurrentOperationType = OperationType::None;
    SharedPtr<Operation> m_CurrentOperation;

    SharedPtr<ExtrudeOperation> m_ExtrudeOp;
    SharedPtr<RevolveOperation> m_RevolveOp;
    SharedPtr<SweepOperation> m_SweepOp;
    SharedPtr<LoftOperation> m_LoftOp;

    SharedPtr<Mesh> m_PreviewMesh;
};

} 
