#ifndef __NEW_CIFTI_MAPPABLE_DATA_FILE_H__
#define __NEW_CIFTI_MAPPABLE_DATA_FILE_H__

#ifdef WORKBENCH_HAVE_C11X

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <memory>

#include "CaretMappableDataFile.h"
#include "CaretObjectTracksModification.h"
#include "CiftiXMLElements.h"
#include "DisplayGroupEnum.h"
#include "VolumeMappableInterface.h"

#include <set>

namespace caret {
    
    class ChartData;
    class ChartDataCartesian;
    class CiftiFile;
    class CiftiInterface;
    class CiftiMappableConnectivityMatrixDataFile;
    class DescriptiveStatistics;
    class FastStatistics;
    class GroupAndNameHierarchyModel;
    class Histogram;
    class SparseVolumeIndexer;

    
    class NewCiftiMappableDataFile :
    public CaretMappableDataFile,
    public VolumeMappableInterface {
        
    protected:
        /**
         * Method for accessing data with CIFTI
         */
        enum DataAccessMethod {
            /**
             * Invalid data access method.
             */
            DATA_ACCESS_METHOD_INVALID,
            /**
             * The data is accessed from CiftiFile using ROW Methods
             * and from the XML using ALONG_COLUMN
             */
            DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN,
            /**
             * Use ALONG_ROW to access CIFTI data
             * which means one is accessing COLUMNS of data
             */
            DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW
        };
        
        /**
         * Method for color mapping a map
         */
        enum ColorMappingMethod {
            /**
             * Invalid color mapping method. 
             */
            COLOR_MAPPING_METHOD_INVALID,
            /**
             * Color mapped with a label table using map's data.
             */
            COLOR_MAPPING_METHOD_LABEL_TABLE,
            /**
             * Use ALL data from file when applying palette color mapping to a map
             */
            COLOR_MAPPING_METHOD_PALETTE_FILE_DATA,
            /**
             * Use only the MAP's data when applying palette color mapping to a map
             */
            COLOR_MAPPING_METHOD_PALETTE_MAP_DATA
        };
        
        /**
         * Type of map data in the file.
         */
        enum FileMapDataType {
            /**
             * Invalid file map data type
             */
            FILE_MAP_DATA_TYPE_INVALID,
            /**
             * The file contains a connectivity matrix.  There is only
             * 'one map' and the data for the map is selectively read
             * and replaced based upon user actions.
             */
            FILE_MAP_DATA_TYPE_MATRIX,
            /**
             * The file contains one or more maps and all maps are loaded
             * when the file is read.
             */
            FILE_MAP_DATA_TYPE_MULTI_MAP
        };
        
        NewCiftiMappableDataFile(const DataFileTypeEnum::Enum dataFileType);
        
    public:
        virtual ~NewCiftiMappableDataFile();
        
        static NewCiftiMappableDataFile* newInstanceForCiftiFileTypeAndSurface(const DataFileTypeEnum::Enum ciftiFileType,
                                                                     const StructureEnum::Enum structure,
                                                                     const int32_t numberOfNodes,
                                                                     AString& errorMessageOut);
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        virtual void clear();
        
        virtual bool isEmpty() const;
        
        virtual bool isMappableToSurfaceStructure(const StructureEnum::Enum structure) const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
//        virtual void setPreferOnDiskReading(const bool& prefer);
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps() const;
        
        virtual bool hasMapAttributes() const;
        
        virtual AString getMapName(const int32_t mapIndex) const;
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual bool isMappedWithPalette() const;
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex);
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues);
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex,
                                                const PaletteFile* paletteFile);
        
        virtual bool isMapColoringValid(const int32_t mapIndex) const;
        
        virtual void getDimensions(int64_t& dimOut1,
                                   int64_t& dimOut2,
                                   int64_t& dimOut3,
                                   int64_t& dimTimeOut,
                                   int64_t& numComponents) const;
        
        virtual void getDimensions(std::vector<int64_t>& dimsOut) const;
        
        virtual void getMapDimensions(std::vector<int64_t> &dim) const;

        virtual const int64_t& getNumberOfComponents() const;
        
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float& coordOut1,
                                  float& coordOut2,
                                  float& coordOut3) const;
        
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float* coordOut) const;
        
        virtual void indexToSpace(const int64_t* indexIn,
                                  float* coordOut) const;
        
        virtual void enclosingVoxel(const float& coordIn1,
                                    const float& coordIn2,
                                    const float& coordIn3,
                                    int64_t& indexOut1,
                                    int64_t& indexOut2,
                                    int64_t& indexOut3) const;
        
        virtual bool indexValid(const int64_t& indexIn1,
                                const int64_t& indexIn2,
                                const int64_t& indexIn3,
                                const int64_t brickIndex = 0,
                                const int64_t component = 0) const;
        
        virtual void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const;
        
        virtual void getVoxelColorsForSliceInMap(const PaletteFile* paletteFile,
                                                 const int32_t mapIndex,
                                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                 const int64_t sliceIndex,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 uint8_t* rgbaOut) const;
        
        virtual void getVoxelColorInMap(const PaletteFile* paletteFile,
                                        const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t mapIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        uint8_t rgbaOut[4]) const;
        
        virtual void getVoxelColorInMapForLabelData(const PaletteFile* paletteFile,
                                                    const std::vector<float>& dataForMap,
                                        const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t mapIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        uint8_t rgbaOut[4]) const;
        
        virtual bool getMapVolumeVoxelValue(const int32_t mapIndex,
                                            const float xyz[3],
                                            int64_t ijkOut[3],
                                            float& numericalValueOut,
                                            bool& numericalValueOutValid,
                                            AString& textValueOut) const;
        
        int64_t getMapDataOffsetForVoxelAtCoordinate(const float coordinate[3],
                                                     const int32_t mapIndex) const;
        
        virtual float getVoxelValue(const float* coordinateIn,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const;
        
        virtual float getVoxelValue(const float coordinateX,
                                    const float coordinateY,
                                    const float coordinateZ,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const;
        
        virtual bool getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                         const float xyz[3],
                                                         int64_t ijkOut[3],
                                                         AString& textOut) const;
        
        std::vector<int32_t> getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        const GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel() const;
        
        virtual bool getMapSurfaceNodeValue(const int32_t mapIndex,
                                            const StructureEnum::Enum structure,
                                            const int nodeIndex,
                                            const int32_t numberOfNodes,
                                            float& numericalValueOut,
                                            bool& numericalValueOutValid,
                                            AString& textValueOut) const;
        
        virtual bool getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                            const StructureEnum::Enum structure,
                                                            const int nodeIndex,
                                                            const int32_t numberOfNodes,
                                                            AString& textOut) const;
        
        int32_t getMappingSurfaceNumberOfNodes(const StructureEnum::Enum structure) const;
        
        bool getSeriesDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t nodeIndex,
                                         std::vector<float>& seriesDataOut) const;
        
        bool getSeriesDataForVoxelAtCoordinate(const float xyz[3],
                                               std::vector<float>& seriesDataOut) const;
        
        virtual bool getMapSurfaceNodeColoring(const int32_t mapIndex,
                                               const StructureEnum::Enum structure,
                                               float* surfaceRGBAOut,
                                               float* dataValuesOut,
                                               const int32_t surfaceNumberOfNodes);
        
        virtual void clearModified();
        
        virtual bool isModifiedExcludingPaletteColorMapping() const;
        
        virtual NiftiTimeUnitsEnum::Enum getMapIntervalUnits() const;
        
        virtual void getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                                float& mapIntervalStepValueOut) const;
        
        virtual bool getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                             float& dataRangeMaximumOut) const;
        
//        bool getStructureAndNodeIndexFromRowIndex(const int64_t rowIndex,
//                                                  StructureEnum::Enum& structureOut,
//                                                  int64_t& nodeIndexOut) const;
//        
//        bool getVoxelIndexAndCoordinateFromRowIndex(const int64_t rowIndex,
//                                                    int64_t ijkOut[3],
//                                                    float xyzOut[3]) const;

        void setMapDataForSurface(const int32_t mapIndex,
                                  const StructureEnum::Enum structure,
                                  const std::vector<float> surfaceMapData) throw (DataFileException);
        
    private:
        
        NewCiftiMappableDataFile(const NewCiftiMappableDataFile&);
        
        NewCiftiMappableDataFile& operator=(const NewCiftiMappableDataFile&);
        
    public:
        
        virtual void getMapData(const int32_t mapIndex,
                                std::vector<float>& dataOut) const;
        
        virtual bool setMapData(const int32_t mapIndex,
                                const std::vector<float>& data);
        
        virtual void getMatrixRGBA(std::vector<float>& rgba, PaletteFile *paletteFile);
    
    protected:
        ChartDataCartesian* helpLoadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                       const int32_t nodeIndex) throw (DataFileException);
        
        ChartDataCartesian* helpLoadChartDataForSurfaceNodeAverage(const StructureEnum::Enum structure,
                                                          const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        ChartDataCartesian* helpLoadChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);
        
        bool helpLoadChartDataMatrixForMap(const int32_t mapIndex,
                                                       int32_t& numberOfRowsOut,
                                                       int32_t& numberOfColumnsOut,
                                                       std::vector<float>& rgbaOut) const;

        class MapContent : public CaretObjectTracksModification {
            
        public:
            
            MapContent(CiftiFile* ciftiFile,
                       const FileMapDataType fileMapDataType,
                       const int32_t mappingDirectionForCiftiXML,
                       const int32_t mapIndex);
            
            ~MapContent();
            
            virtual void clearModified();
            
            virtual bool isModified() const;
            
            void invalidateColoring();
            
            void updateColoring(const std::vector<float>& data,
                                const PaletteFile* paletteFile);
            
            AString getName() const;
            
            void setName(const AString& name);
            
            /** The CIFTI file pointer */
            CiftiFile *m_ciftiFile;
            
            /** Maps or Matrix file */
            const FileMapDataType m_fileMapDataType;
            
            /** Direction for obtaining mapping information (CiftiXML::ALONG_ROW, etc) */
            const int32_t m_mappingDirectionForCiftiXML;
            
            /** Index of this map */
            const int32_t m_mapIndex;
            
            /** Count of data elements in map. */
            int64_t m_dataCount = 0;
            
            /** Metadata for the map. Points to data in CiftiFile so DO NOT delete */
            GiftiMetaData* m_metadata = nullptr;
            
            /** Palette color mapping for map. Points to data in CiftiFile so DO NOT delete  */
            PaletteColorMapping* m_paletteColorMapping = nullptr;
            
            /** Label table for map. Points to data in CiftiFile so DO NOT delete  */
            GiftiLabelTable* m_labelTable = nullptr;

            /** Indicates data is mapped with a lable table */
            bool m_dataIsMappedWithLabelTable = false;
            
            /** RGBA coloring for map */
            std::vector<uint8_t> m_rgba;
            
            /** RGBA coloring is valid */
            bool m_rgbaValid = false;
            
            /** descriptive statistics for map */
            std::unique_ptr<DescriptiveStatistics> m_descriptiveStatistics;
            
            /** fast statistics for map */
            std::unique_ptr<FastStatistics> m_fastStatistics;
            
            /** histogram for map */
            std::unique_ptr<Histogram> m_histogram;
        
        private:
            /** Name of map */
            AString m_name;
        };
        
        void clearPrivate();
        
        void initializeAfterReading(const AString& filename) throw (DataFileException);
        
        static AString ciftiIndexTypeToName(const IndicesMapToDataType ciftiIndexType);
        
        void validateKeysAndLabels() const;
        
        virtual void validateAfterFileReading() throw (DataFileException);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
        bool getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                             const int64_t surfaceNumberOfNodes,
                                                             std::vector<int64_t>& dataIndicesForNodes) const;
        /**
         * Point to the CIFTI file object.
         */
        std::unique_ptr<CiftiFile> m_ciftiFile;
        
        /**
         * Method used when reading data from the file.
         */
        DataAccessMethod m_dataReadingAccessMethod = DATA_ACCESS_METHOD_INVALID;
        
        /** Direction for obtaining mapping information (CiftiXML::ALONG_ROW, etc) */
        int32_t m_dataReadingDirectionForCiftiXML = -1;
        
        /**
         * Method used when mapping loaded data to brainordinates.
         */
        DataAccessMethod m_dataMappingAccessMethod = DATA_ACCESS_METHOD_INVALID;
        
        /** Direction for obtaining mapping information (CiftiXML::ALONG_ROW, etc) */
        int32_t m_dataMappingDirectionForCiftiXML = -1;
        
        /**
         * Method used when mapping data to colors.
         */
        ColorMappingMethod m_colorMappingMethod = COLOR_MAPPING_METHOD_INVALID;
        
        /**
         * Type of data in the file's map(s).
         */
        FileMapDataType m_fileMapDataType = FILE_MAP_DATA_TYPE_INVALID;
        
        /** Contains data related to each map */
        std::vector<MapContent*> m_mapContent;
        
        /** True if the file contains surface data */
        bool m_containsSurfaceData = false;
        
        /** True if the file contains surface data */
        bool m_containsVolumeData = false;
        
        float m_mappingTimeStart = 0.0;
        
        float m_mappingTimeStep = 0.0;
        
        NiftiTimeUnitsEnum::Enum m_mappingTimeUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
                
        /** Fast conversion of IJK to data offset */
        std::unique_ptr<SparseVolumeIndexer> m_voxelIndicesToOffset;
        
//        /** How to read data from file */
//        FileReading m_fileReading;
//        
//        /** Required type of data along the row (dimension 0) */
//        const IndicesMapToDataType m_requiredRowIndexType;
//        
//        /** Required type of data along the column (dimension 1) */
//        const IndicesMapToDataType m_requiredColumnIndexType;
//        
//        /** Location of brainordinate mapped data */
//        const DataAccess m_brainordinateMappedDataAccess;
//        
//        /** Location of series data */
//        const DataAccess m_seriesDataAccess;
//        
//        /** Contains data related to each map */
//        std::vector<MapContent*> m_mapContent;
//        
//        /** True if the file contains volume data */
//        bool m_containsVolumeData;
//        
//        /** The CIFTI interface (could be local file or on network) */
//        CaretPointer<CiftiInterface> m_ciftiInterface;
//        
//        /** The file's metadata. */
//        CaretPointer<GiftiMetaData> m_metadata;
//        
//        /** CIFTI Facade for simplifying access to CIFTI data */
//        CaretPointer<CiftiFacade> m_ciftiFacade;
//        
//        /** Dimensions of volume data */
//        int64_t m_volumeDimensions[5];
        
        /** Holds class and name hierarchy used for display selection */
        mutable std::unique_ptr<GroupAndNameHierarchyModel> m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        mutable bool m_forceUpdateOfGroupAndNameHierarchy = true;

//        std::vector<int64_t> m_ciftiDimensions;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __NEW_CIFTI_MAPPABLE_DATA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEW_CIFTI_MAPPABLE_DATA_FILE_DECLARE__
    
} // namespace

#endif // WORKBENCH_HAVE_C11X

#endif  //__NEW_CIFTI_MAPPABLE_DATA_FILE_H__