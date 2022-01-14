using System.Collections;
using System.Collections.Generic;
using ML;
using UnityEngine;

public class MLPoint : MonoBehaviour
{

    [SerializeField] private MeshRenderer outputRenderer;
    [SerializeField] private MeshRenderer expectedRenderer;
    public void setMaterial(Material m)
    {
        outputRenderer.material = m;
        expectedRenderer.material = m;
    }

    public void setMaterials(Material output, Material expected)
    {
        
        outputRenderer.material = output;
        expectedRenderer.material = expected;
    }

    public Material getMaterial()
    {
        return expectedRenderer.material;
    }

    public void SetVisible(bool val)
    {
        outputRenderer.enabled = val;
        expectedRenderer.enabled = val;
    }
}
